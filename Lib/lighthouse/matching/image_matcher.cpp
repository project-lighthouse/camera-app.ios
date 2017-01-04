//
//  image_matcher.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "image_matcher.hpp"
#include <uuid/uuid.h>

namespace lighthouse {

ImageMatcher::ImageMatcher(int32_t aNumberOfFeatures, float aRatioTestK, float aHistogramWeight): mDB(),
                                                       mKeypointDetector(cv::ORB::create(aNumberOfFeatures)),
                                                       mMatcher(new cv::BFMatcher(cv::NORM_HAMMING)),
                                                       mRatioTestK(aRatioTestK),
                                                       mHistogramWeight(aHistogramWeight) {}

ImageDescription ImageMatcher::GetDescription(const cv::Mat &aInputFrame) const {
    std::vector<cv::Mat> rgbaChannels(4);
    cv::split(aInputFrame, rgbaChannels);

    // Detect image keypoints and compute descriptors for all of them.
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors, histogram;

    mKeypointDetector->detectAndCompute(aInputFrame, rgbaChannels[3], keypoints, descriptors);

    // Calculate color histogram for the image. But there is no need in calculating of the color histogram if we didn't
    // find any keypoint.
    if (keypoints.size() > 0) {
        const int channels[] = {0, 1, 2};
        const int histogramSize[] = {8, 8, 8};
        float colorRange[] = {0, 256};
        const float *ranges[] = {colorRange, colorRange, colorRange};
        cv::calcHist(&aInputFrame, 1, channels, cv::Mat(), histogram, 3, histogramSize, ranges);

        cv::normalize(histogram, histogram);
    }

    // Generate unique ImageDescription Id.
    uuid_t uuid;
    uuid_generate_random(uuid);

    char uuidString[37];
    uuid_unparse(uuid, uuidString);

    return ImageDescription(uuidString, keypoints, descriptors, histogram);
}

void ImageMatcher::AddToDB(const ImageDescription &aDescription) {
    mDB.push_back(aDescription);
}

std::vector<std::tuple<float, ImageDescription>> ImageMatcher::Match(const ImageDescription &aDescription) const {
    std::vector<std::tuple<float, ImageDescription>> matchedDescriptions;

    for (const ImageDescription description : mDB) {
        std::vector<std::vector<cv::DMatch>> matches = {};
        mMatcher->knnMatch(aDescription.GetDescriptors(), description.GetDescriptors(), matches, 2);

        if (matches.size() == 0) {
            continue;
        }

        // Apply ratio test: if the best match is significantly better than the second best match then we consider it to
        // be a good match. Note that the absolute distance of the matches does not matter just their relative amounts.
        uint32_t numberOfGoodMatches = 0;
        for (const std::vector<cv::DMatch> matchPair : matches) {
            if (matchPair.size() == 2 && matchPair[0].distance < mRatioTestK * matchPair[1].distance) {
                numberOfGoodMatches++;
            }
        }

        // If the two images have similar numbers of keypoints this number will be high and will increase the score.
        // featureRatio = 1 - abs(aDescription.GetDescriptors.size() - description.GetDescriptors().size()) /
        // description.GetDescriptors().size();

        // If most of the feature matches are good ones this ratio will be high and will increase the score.
        const float goodMatchRatio = (float) numberOfGoodMatches / (float) matches.size();

        // Both of the numbers above are between 0 and 1. We take their product and multiply by 100 to create a score
        // between 0 and 100. Kind of a match percentage.
        float score = goodMatchRatio * 100 ; // featureRatio * goodMatchRatio * 100

        // Now boost the score based on how well the histograms match.
        if (mHistogramWeight > 0) {
            double histogramCorrelation = cv::compareHist(aDescription.GetHistogram(), description.GetHistogram(),
                    cv::HISTCMP_CORREL);
            score += mHistogramWeight * histogramCorrelation;
        }

        matchedDescriptions.push_back(std::make_tuple(score, description));
    }

    // Sort matched description by score (first element of the tuple).
    std::sort(matchedDescriptions.begin(), matchedDescriptions.end(),
            [](std::tuple<float, ImageDescription> a, std::tuple<float, ImageDescription> b) {
                return std::get<0>(b) < std::get<0>(a);
            });

    return matchedDescriptions;
}

} // namespace lighthouse
