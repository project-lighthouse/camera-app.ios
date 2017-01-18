//
//  image_matcher.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include <uuid/uuid.h>

#include "image_matcher.hpp"
#include "exceptions.hpp"

namespace lighthouse {

ImageMatcher::ImageMatcher(ImageMatchingSettings aSettings)
    : mSettings(aSettings), mDB(), mKeypointDetector(cv::ORB::create(aSettings.mNumberOfFeatures)),
      mMatcher(new cv::BFMatcher(cv::NORM_HAMMING)) {
}

ImageDescription ImageMatcher::GetDescription(const cv::Mat &aInputFrame) const {
  // Use alpha channel as a mask.
  std::vector<cv::Mat> rgbaChannels(4);
  cv::split(aInputFrame, rgbaChannels);
  cv::Mat mask = rgbaChannels[3];

  // Detect image keypoints and compute descriptors for all of them.
  std::vector<cv::KeyPoint> keypoints;
  cv::Mat descriptors, histogram;

  mKeypointDetector->detectAndCompute(aInputFrame, mask, keypoints, descriptors);

  uint32_t keypointsCount = keypoints.size();

  fprintf(stderr, "ImageMatcher::GetDescription() number of extracted keypoints: %i.\n", keypointsCount);

  if (keypointsCount < mSettings.mMinNumberOfFeatures) {
    throw ImageQualityException("Image does not have enough keypoints.", ImageQualityExceptionCode::NotEnoughKeyPoints);
  }

  // Calculate color histogram for the image.
  const int channels[] = {0, 1, 2};
  const int histogramSize[] = {8, 8, 8};
  float colorRange[] = {0, 256};
  const float *ranges[] = {colorRange, colorRange, colorRange};

  cv::calcHist(&aInputFrame, 1, channels, cv::Mat(), histogram, 3, histogramSize, ranges);
  cv::normalize(histogram, histogram);

  // Generate unique ImageDescription Id.
  uuid_t uuid;
  uuid_generate_random(uuid);

  char uuidString[37];
  uuid_unparse(uuid, uuidString);

  fprintf(stderr, "ImageMatcher::GetImageDescription() created new image description with ID: %s.\n", uuidString);

  return ImageDescription(uuidString, keypoints, descriptors, histogram);
}

const ImageDescription &ImageMatcher::GetDescription(const std::string &id) const {
  return mDB.find(id)->second;
}

void ImageMatcher::AddToDB(const ImageDescription &aDescription) {
  mDB.insert(std::make_pair(aDescription.GetId(), aDescription));
}

std::tuple<std::vector<std::vector<cv::DMatch>>, std::vector<std::vector<cv::DMatch>>> ImageMatcher::Match(
    const ImageDescription &aFirstDescription, const ImageDescription &aSecondDescription) const {
  std::vector<std::vector<cv::DMatch>> matches;
  mMatcher->knnMatch(aFirstDescription.GetDescriptors(), aSecondDescription.GetDescriptors(), matches, 2);

  // Partition matches collection in "good" and "bad" matches.
  float ratioTestK = mSettings.mRatioTestK;
  auto matchSplitIterator = std::partition(matches.begin(), matches.end(),
      [&ratioTestK](std::vector<cv::DMatch> matchPair) {
        return matchPair.size() == 2 && matchPair[0].distance < ratioTestK * matchPair[1].distance;
      });

  std::vector<std::vector<cv::DMatch>> goodMatches(matches.begin(), matchSplitIterator);
  std::vector<std::vector<cv::DMatch>> badMatches(matchSplitIterator, matches.end());

  return std::make_tuple(goodMatches, badMatches);
}

std::vector<std::tuple<float, ImageDescription>> ImageMatcher::FindMatches(const ImageDescription &aDescription) const {
  std::vector<std::tuple<float, ImageDescription>> matchedDescriptions;

  for (const auto &descriptionPair : mDB) {
    const ImageDescription &description = descriptionPair.second;

    auto matchesTuple = Match(aDescription, description);

    uint32_t goodMatchesCount = std::get<0>(matchesTuple).size();
    uint32_t totalMatchesCount = goodMatchesCount + std::get<1>(matchesTuple).size();

    if (goodMatchesCount == 0) {
      continue;
    }

    // If the two images have similar numbers of keypoints this number will be high and will increase the score.
    // featureRatio = 1 - abs(aDescription.GetDescriptors.size() - description.GetDescriptors().size()) /
    // description.GetDescriptors().size();

    // If most of the feature matches are good ones this ratio will be high and will increase the score.
    const float goodMatchRatio = (float) goodMatchesCount / (float) totalMatchesCount;

    // Both of the numbers above are between 0 and 1. We take their product and multiply by 100 to create a score
    // between 0 and 100. Kind of a match percentage.
    float score = goodMatchRatio * 100; // featureRatio * goodMatchRatio * 100

    // Now boost the score based on how well the histograms match.
    if (mSettings.mHistogramWeight > 0) {
      double histogramCorrelation = cv::compareHist(aDescription.GetHistogram(), description.GetHistogram(),
          cv::HISTCMP_CORREL);
      score += mSettings.mHistogramWeight * histogramCorrelation;
    }

    fprintf(stderr, "ImageMatcher::FindMatches() %s vs %s: total matches (%i), good matches (%i), score (%f).\n",
        aDescription.GetId().c_str(), description.GetId().c_str(), totalMatchesCount, goodMatchesCount, score);

    if (score >= mSettings.mMatchingScoreThreshold) {
      matchedDescriptions.push_back(std::make_tuple(score, description));
    }
  }

  // Sort matched description by score (first element of the tuple).
  std::sort(std::begin(matchedDescriptions), std::end(matchedDescriptions),
      [](const std::tuple<float, ImageDescription> &a, const std::tuple<float, ImageDescription> &b) {
        return std::get<0>(b) < std::get<0>(a);
      });

  return matchedDescriptions;
}

} // namespace lighthouse
