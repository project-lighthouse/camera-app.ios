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

Lighthouse::Lighthouse(int32_t aNumberOfFeatures) {
    this->mFeatureDetector = ORB::create(aNumberOfFeatures);
    this->mMatcher = new BFMatcher(NORM_HAMMING);
}

void Lighthouse::DrawKeypoints(const Mat &aInputFrame, Mat &aOutputFrame) {
    vector<KeyPoint> keypoints;
    this->mFeatureDetector->detect(aInputFrame, keypoints);

    // We can't draw keypoints on the BGRA image.
    Mat bgrInputFrame;
    cvtColor(aInputFrame, bgrInputFrame, cv::COLOR_BGRA2BGR);

    drawKeypoints(bgrInputFrame, keypoints, aOutputFrame, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
}

const size_t Lighthouse::ExtractFeatures(const Mat &aInputFrame) {
    Mat grayImage;
    cvtColor(aInputFrame, grayImage, cv::COLOR_BGRA2GRAY);

    vector<Mat> rgbChannels(3);
    split(aInputFrame, rgbChannels);

    vector<KeyPoint> keypoints;
    Mat descriptors;
    this->mFeatureDetector->detectAndCompute(grayImage, rgbChannels[2], keypoints, descriptors);

    return keypoints.size();
}