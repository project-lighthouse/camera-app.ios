//
//  image_description.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 23/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include <fstream>

#include <cereal/archives/binary.hpp>

#include "serialization.hpp"
#include "image_description.hpp"

namespace lighthouse {

ImageDescription::ImageDescription(std::string aId, std::vector<cv::KeyPoint> aKeypoints, cv::Mat aDescriptors,
        cv::Mat aHistogram): mId(), mKeypoints(aKeypoints), mDescriptors(aDescriptors), mHistogram(aHistogram) {}

const std::string ImageDescription::GetId() {
    return mId;
}

const std::vector<cv::KeyPoint> ImageDescription::GetKeypoints() {
    return mKeypoints;
}

const cv::Mat ImageDescription::GetDescriptors() {
    return mDescriptors;
}

const cv::Mat ImageDescription::GetHistogram() {
    return mHistogram;
}

void ImageDescription::Save(ImageDescription aDescription, std::string aPath) {
    std::ofstream outputStream(aPath, std::ios::binary);
    cereal::BinaryOutputArchive ar(outputStream);

    ar(aDescription);
}

ImageDescription ImageDescription::Load(std::string aPath) {
    std::ifstream inputStream(aPath, std::ios::binary);
    cereal::BinaryInputArchive archive(inputStream);

    std::string id;
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors, histogram;

    ImageDescription description = ImageDescription(id, keypoints, descriptors, histogram);
    archive(description);

    return description;
}
} // namespace lighthouse
