//
//  video.cpp
//  Lighthouse Camera
//
//  Created by David Teller on 05/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include "video.hpp"
#include "feedback.hpp"

#include "opencv2/videoio.hpp"
#include "opencv2/core/cvstd.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <thread>

using namespace lighthouse;

lighthouse::Camera::Camera():
    mCaptureThread(nullptr)
{ }

void lighthouse::Camera::RunCaptureForRecord() {
    fprintf(stderr, "RunCaptureForRecord() start\n");
    auto capture = cv::Ptr<cv::VideoCapture>(new cv::VideoCapture::VideoCapture());
    if (!capture->open(0)) {
        fprintf(stderr, "RunCaptureForRecord() could not open camera 0\n");
        return;
    }
    for (uint64_t i = 0; i < 200; ++i) {
        cv::Mat frame;
        if (!capture->read(frame)) {
            fprintf(stderr, "RunCaptureForRecord() skipping frame %llu\n", i);
            continue;
        }
        fprintf(stderr, "RunCaptureForRecord() got frame %llu\n", i);
        Feedback::ReceivedFrame(frame);
    }
    fprintf(stderr, "RunCaptureForRecord() stop\n");
    // FIXME: Somehow reset `mCaptureThread` to `nullptr`.
}

void lighthouse::Camera::CaptureForRecord() {
    fprintf(stderr, "Lighthouse::CaptureForRecord() start\n");
    // FIXME: We assume that this is always called from the UI thread. How can we check that?
    if (!mCaptureThread) {
        mCaptureThread = new std::thread(Camera::RunCaptureForRecord);
    }
    fprintf(stderr, "Lighthouse::CaptureForRecord() stop\n");

}
