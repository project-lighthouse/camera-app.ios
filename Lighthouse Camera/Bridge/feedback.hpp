//
//  feedback.hpp
//  Lighthouse Camera
//
//  Created by David Teller on 06/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//
//
// C++ callbacks (implemented in Obj-C) to let C++ code provide visual/auditory feedback.


#ifndef feedback_hpp
#define feedback_hpp

#include <stdio.h>

#include <opencv2/opencv.hpp> // Must be imported before Cocoa


class Feedback {
public:
    static void ReceivedFrame(cv::Mat& frame);
    static void OperationComplete();
};

#endif /* feedback_hpp */
