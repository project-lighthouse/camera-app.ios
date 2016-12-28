//
//  image_description.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 23/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#ifndef image_description_hpp
#define image_description_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <cereal/access.hpp>

namespace lighthouse {

class ImageDescription {
public:
    ImageDescription(std::vector<cv::KeyPoint> keypoints, cv::Mat descriptors, cv::MatND histogram);
    const std::vector<cv::KeyPoint> GetKeypoints();
    const cv::Mat GetDescriptors();
    const cv::MatND GetHistogram();
    void Save();

private:
    std::vector<cv::KeyPoint> mKeypoints;
    cv::Mat mDescriptors;
    cv::MatND mHistogram;

    friend class cereal::access;
    template<class Archive> void save(Archive& archive) const;
    template<class Archive> void load(Archive& archive);
};

} // namespace lighthouse

#endif /* image_description_hpp */
