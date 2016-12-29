//
//  serialization.h
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 29/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#ifndef serialization_hpp
#define serialization_hpp

#include <numeric>
#include <opencv2/opencv.hpp>

namespace cv {
/**
 * Serialize cv::Mat into Cereal archive.
 *
 * @param[in] aArchive The Cereal archive to serialize to.
 * @param[in] aMatrix The cv::Mat instance to serialize.
 */
template<class Archive>
void save(Archive &aArchive, const cv::Mat &aMatrix) {
    const bool continuous = aMatrix.isContinuous();
    if (!continuous) {
        throw std::invalid_argument("Non-continuous matrix (de)serialization is not yet supported!");
    }

    std::vector<int> matrixSize(aMatrix.dims);
    for(int i = 0; i < aMatrix.dims; ++i) {
        matrixSize[i] = aMatrix.size[i];
    }

    aArchive(aMatrix.dims, matrixSize, aMatrix.type(), continuous,
            cereal::binary_data(aMatrix.ptr(), aMatrix.total() * aMatrix.elemSize()));
}

/**
 * De-serialize cv::Mat from a Cereal archive.
 *
 * @param[in] aArchive The archive to deserialize from.
 * @param[in] aMatrix The cv::Mat instance to deserialize.
 */
template<class Archive>
void load(Archive &aArchive, cv::Mat &aMatrix) {
    int dims, type;
    bool continuous;
    std::vector<int> matrixSize;

    aArchive(dims, matrixSize, type, continuous);

    if (!continuous) {
        throw std::invalid_argument("Non-continuous matrix (de)serialization is not yet supported!");
    }

    aMatrix.create(dims, &matrixSize[0], type);

    aArchive(cereal::binary_data(aMatrix.ptr(), aMatrix.total() * aMatrix.elemSize()));
}

/**
 * Serialize cv::KeyPoint into Cereal archive.
 *
 * @param[in] aArchive The Cereal archive to serialize to.
 * @param[in] aKeyPoint The cv::KeyPoint instance to serialize.
 */
template<class Archive>
void save(Archive &aArchive, const cv::KeyPoint &aKeyPoint) {
    aArchive(aKeyPoint.pt.x, aKeyPoint.pt.y, aKeyPoint.size, aKeyPoint.angle, aKeyPoint.response, aKeyPoint.octave,
            aKeyPoint.class_id);
}

/**
 * De-serialize cv::KeyPoint from a Cereal archive.
 *
 * @param[in] aArchive The archive to deserialize from.
 * @param[in] aKeyPoint The cv::KeyPoint instance to deserialize.
 */
template<class Archive>
void load(Archive &aArchive, cv::KeyPoint &aKeyPoint) {
    float x, y, size, angle, response;
    int octave, class_id;

    aArchive(x, y, size, angle, response, octave, class_id);

    aKeyPoint.pt.x = x;
    aKeyPoint.pt.y = y;
    aKeyPoint.size = size;
    aKeyPoint.angle = angle;
    aKeyPoint.response = response;
    aKeyPoint.octave = octave;
    aKeyPoint.class_id = class_id;
}
} // namespace cv

#endif /* serialization_hpp */
