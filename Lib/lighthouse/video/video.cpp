//
//  video.cpp
//  Lighthouse Camera
//
//  Created by David Teller on 05/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include "video.hpp"
#include "feedback.hpp"
#include "filesystem.hpp"

#include "opencv2/videoio.hpp"
#include "opencv2/core/cvstd.hpp"
#include "opencv2/highgui/highgui.hpp"

#import <TargetConditionals.h>

#include <thread>
#include <stdint.h>

using namespace lighthouse;
using namespace cv;

lighthouse::Camera::Camera():
    mCaptureThread(nullptr)
{ }

void lighthouse::Camera::RunCaptureForRecord() {
    fprintf(stderr, "RunCaptureForRecord() start\n");
    auto capture = Ptr<VideoCapture>(new VideoCapture());
    
#if TARGET_IPHONE_SIMULATOR
    // Simulator specific code

    const uint64_t FRAMES_LEN = UINT64_MAX; // No real limit, run until the end of the video.

    const std::string resourceName("box");
    const std::string resourceType("mp4");
    std::string path = Filesystem::GetResourcePath(resourceName, resourceType);
    if (!capture->open(path)) {
        fprintf(stderr, "RunCaptureForRecord() could not open bundled video\n");
        return;
    }

#else // TARGET_IPHONE_SIMULATOR
    // Device specific code

    const uint64_t FRAMES_LEN = 2000;

    if (!capture->open(0)) {
        fprintf(stderr, "RunCaptureForRecord() could not open camera 0\n");
        return;
    }
    
#endif // TARGET_IPHONE_SIMULATOR

    Ptr<BackgroundSubtractorMOG2> subtracted(createBackgroundSubtractorMOG2());
    Mat mask;
    for (uint64_t i = 0; i < FRAMES_LEN; ++i) {
        Mat frame;
        if (!capture->read(frame)) {
            fprintf(stderr, "RunCaptureForRecord() out of frames at %llu\n", i);
            break;
        }
        fprintf(stderr, "RunCaptureForRecord() got frame %llu\n", i);
        subtracted->apply(frame, mask);
        Feedback::ReceivedFrame(mask);
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
