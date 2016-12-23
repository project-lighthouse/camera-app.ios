//
//  image_matcher.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "image_matcher.hpp"

namespace lighthouse {

ImageMatcher::ImageMatcher(int32_t aNumberOfFeatures) {
    this->mKeypointDetector = cv::ORB::create(aNumberOfFeatures);
    this->mMatcher = new cv::BFMatcher(cv::NORM_HAMMING);
}

const ImageDescription ImageMatcher::GetDescription(const cv::Mat &aInputFrame) {
    std::vector<cv::Mat> rgbaChannels(4);
    cv::split(aInputFrame, rgbaChannels);

    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    this->mKeypointDetector->detectAndCompute(aInputFrame, rgbaChannels[3], keypoints, descriptors);

    return ImageDescription(keypoints, descriptors);
}

} // namespace lighthouse
