//
//  image_description.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 23/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include <fstream>
#include "image_description.hpp"
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

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

void ImageDescription::Save() {
    std::ofstream outputStream("description.cereal", std::ios::binary);
    cereal::BinaryOutputArchive archive(outputStream);

    archive(std::unique_ptr<ImageDescription>(this));
}


/**
 * Serialize Image Description into Cereal archive.
 *
 * @param[in] archive The Cereal archive to serialise to.
 */
template<class Archive>
void ImageDescription::save(Archive &archive) const {
    const int rows = mDescriptors.rows;
    const int cols = mDescriptors.cols;
    const bool continuous = mDescriptors.isContinuous();
    const size_t elementSize = mDescriptors.elemSize();

    archive(rows, cols, mDescriptors.type(), continuous);

    if (continuous) {
        archive(cereal::binary_data(mDescriptors.ptr(), rows * cols * elementSize));
        return;
    }

    const size_t rowSize = cols * elementSize;
    for (int i = 0; i < rows; i++) {
        archive(cereal::binary_data(mDescriptors.ptr(i), rowSize));
    }
}

/**
 * De-serialize Image Description from a Cereal archive.
 *
 * @param[in] archive The archive to deserialize from.
 */
template<class Archive>
void ImageDescription::load(Archive &archive) {
    int rows, cols, type;
    bool continuous;

    archive(rows, cols, type, continuous);
    mDescriptors.create(rows, cols, type);

    const size_t elementSize = mDescriptors.elemSize();

    if (continuous) {
        archive(cereal::binary_data(mDescriptors.ptr(), rows * cols * elementSize));
        return;
    }

    const size_t rowSize = cols * elementSize;
    for (int i = 0; i < rows; i++) {
        archive(cereal::binary_data(mDescriptors.ptr(i), rowSize));
    }
}

} // namespace lighthouse
