//
//  image_matcher.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#ifndef image_matcher_hpp
#define image_matcher_hpp

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include "image_description.hpp"

namespace lighthouse {

// Describes all possible configurable values that can be passed to the ImageMatcher.
struct ImageMatchingSettings {
  // Number of features to extract used in ORB detector.
  uint32_t mNumberOfFeatures;
  // Minimal number of keypoints that should be extracted from the target image to be considered as good enough sample.
  uint32_t mMinNumberOfFeatures;
  // Minimal matching score threshold below which we consider image as not matched.
  float mMatchingScoreThreshold;
  // Secondary matches must have a score at least this fraction of the best match.
  float mRatioTestK;
  // How much weight to give to histogram correlation when matching images.
  float mHistogramWeight;
};

class ImageMatcher {
public:
  ImageMatcher(ImageMatchingSettings aSettings);

  ImageDescription GetDescription(const cv::Mat &aInputFrame) const;

  const ImageDescription &GetDescription(const std::string &id) const;

  std::tuple<std::vector<std::vector<cv::DMatch>>, std::vector<std::vector<cv::DMatch>>> Match(
      const ImageDescription &aFirstDescription, const ImageDescription &aSecondDescription) const;

  std::vector<std::tuple<float, ImageDescription>> FindMatches(const ImageDescription &aDescription) const;

  void AddToDB(const ImageDescription &aDescription);

private:
  cv::Ptr<cv::Feature2D> mKeypointDetector;
  cv::Ptr<cv::DescriptorMatcher> mMatcher;
  std::unordered_map<std::string, ImageDescription> mDB;
  ImageMatchingSettings mSettings;
};

} // namespace lighthouse

#endif /* image_matcher_hpp */
