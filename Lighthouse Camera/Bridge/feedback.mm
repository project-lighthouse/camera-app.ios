//
//  feedback.cpp
//  Lighthouse Camera
//
//  Created by David Teller on 06/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#include <opencv2/opencv.hpp> // Must be imported before Cocoa
#include "image.hpp"
#include "feedback.hpp"
#include "bridge.h"

#import <Foundation/Foundation.h>

SEL showFrameSelector = @selector(showFrame:);

void
Feedback::ReceivedFrame(cv::Mat& frame) {
    fprintf(stderr, "Feedback::ReceivedFrame() got frame\n");
    UIImage* image = matrixToImage(frame); // FIXME: Who owns that?
    [sViewController performSelectorOnMainThread:showFrameSelector withObject:image waitUntilDone:false];
    fprintf(stderr, "Feedback::ReceivedFrame() dispatched frame\n");
}
