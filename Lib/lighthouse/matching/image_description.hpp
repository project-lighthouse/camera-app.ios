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

namespace lighthouse {

class ImageDescription {
public:
    ImageDescription(std::vector<cv::KeyPoint> keypoints, cv::Mat descriptors);
    const std::vector<cv::KeyPoint> GetKeypoints();
    const cv::Mat GetDescriptors();

private:
    std::vector<cv::KeyPoint> mKeypoints;
    cv::Mat mDescriptors;
};

} // namespace lighthouse

#endif /* image_description_hpp */
