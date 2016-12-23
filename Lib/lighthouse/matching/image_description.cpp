//
//  image_description.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 23/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "image_description.hpp"

namespace lighthouse {

ImageDescription::ImageDescription(std::vector<cv::KeyPoint> keypoints, cv::Mat descriptors, cv::MatND histogram) {
    this->mKeypoints = keypoints;
    this->mDescriptors = descriptors;
    this->mHistogram = histogram;
}

const std::vector<cv::KeyPoint> ImageDescription::GetKeypoints() {
    return this->mKeypoints;
}

const cv::Mat ImageDescription::GetDescriptors() {
    return this->mDescriptors;
}

const cv::MatND ImageDescription::GetHistogram() {
    return this->mHistogram;
}

} // namespace lighthouse
