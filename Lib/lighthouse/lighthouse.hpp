//
//  lighthouse.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#ifndef lighthouse_hpp
#define lighthouse_hpp

#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>

class Lighthouse {
public:
    const bool contours(const cv::Mat& inputFrame, cv::Mat& outputFrame);
};

#endif /* lighthouse_hpp */
