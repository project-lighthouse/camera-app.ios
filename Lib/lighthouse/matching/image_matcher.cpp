//
//  image_matcher.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "image_matcher.hpp"

namespace lighthouse {

ImageMatcher::ImageMatcher(int32_t aNumberOfFeatures): mKeypointDetector(cv::ORB::create(aNumberOfFeatures)),
                                                       mMatcher(new cv::BFMatcher(cv::NORM_HAMMING)) {}

const ImageDescription ImageMatcher::GetDescription(const cv::Mat &aInputFrame) {
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

    return ImageDescription(keypoints, descriptors, histogram);
}

} // namespace lighthouse
