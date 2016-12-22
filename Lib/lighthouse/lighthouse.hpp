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
#include <opencv2/features2d.hpp>

class Lighthouse {
public:
    Lighthouse(int32_t aNumberOfFeatures);

    void DrawKeypoints(const cv::Mat &aInputFrame, cv::Mat &aOutputFrame);
    const size_t ExtractFeatures(const cv::Mat& aInputFrame);

private:
    cv::Ptr<cv::ORB> mFeatureDetector;
    cv::Ptr<cv::BFMatcher> mMatcher;
};

#endif /* lighthouse_hpp */
