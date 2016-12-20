//
//  lighthouse.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "lighthouse.hpp"

using namespace std;
using namespace cv;

const bool Lighthouse::contours(const cv::Mat& inputFrame, cv::Mat& outputFrame) {
    Mat gray;
    cv::cvtColor(inputFrame, gray, cv::COLOR_BGRA2GRAY);
    
    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150);
    
    std::vector<std::vector<cv::Point>> contours;
    
    cv::findContours(edges, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    
    inputFrame.copyTo(outputFrame);
    cv::drawContours(outputFrame, contours, -1, cv::Scalar(0,200,0));
    
    return true;
}
