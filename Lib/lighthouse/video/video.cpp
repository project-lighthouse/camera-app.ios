//
//  video.cpp
//  Lighthouse Camera
//
//  Created by David Teller on 05/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include "feedback.hpp"
#include "filesystem.hpp"
#include "lighthouse.hpp"
#include "video.hpp"

#include "opencv2/videoio.hpp"
#include "opencv2/core/cvstd.hpp"
#include "opencv2/highgui/highgui.hpp"

#import <TargetConditionals.h>

#include <stdint.h>
#include <atomic>

using namespace lighthouse;
using namespace cv;

lighthouse::Camera::Camera()
{ }

void
Camera::CaptureForIdentification(std::atomic_int *aState) {
    // FIXME: TODO
}

void
Camera::CaptureForRecord(std::atomic_int *aState) {
    // Current implementation simply captures video.
    // FIXME: Post-process video.
    // FIXME: Trigger audio feedback.
    fprintf(stderr, "RunCaptureForRecord() start\n");
    auto capture = Ptr<VideoCapture>(new VideoCapture());
    
#if TARGET_IPHONE_SIMULATOR
    // Simulator specific code

    const std::string resourceName("box");
    const std::string resourceType("mp4");
    std::string path = Filesystem::GetResourcePath(resourceName, resourceType);
    if (!capture->open(path)) {
        fprintf(stderr, "RunCaptureForRecord() could not open bundled video\n");
        return;
    }

#else // TARGET_IPHONE_SIMULATOR
    // Device specific code

    if (!capture->open(0)) {
        fprintf(stderr, "RunCaptureForRecord() could not open camera 0\n");
        return;
    }
    
#endif // TARGET_IPHONE_SIMULATOR

    Ptr<BackgroundSubtractorMOG2> subtracted(createBackgroundSubtractorMOG2());
    Mat mask;
    while (true) {
        if (aState->load() != (int)Task::RECORD) {
            // We have been asked to stop. Bailout asap.
            return;
        }
        Mat frame;
        if (!capture->read(frame)) {
            // Nothing more to capture.
            break;
        }
        subtracted->apply(frame, mask);
        Feedback::ReceivedFrame(mask);
    }
    Feedback::OperationComplete();
    fprintf(stderr, "RunCaptureForRecord() stop\n");
}
