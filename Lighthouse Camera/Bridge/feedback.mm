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
#include "feedback.h"
#include "player.hpp"
#include "bridge.h"


#import <Foundation/Foundation.h>

SEL showFrameSelector = @selector(showFrame:);
SEL showTextSelector = @selector(showText:);
SEL operationCompleteSelector =@selector(operationComplete);


void
Feedback::ReceivedFrame(const char* info, cv::Mat& frame) {
  fprintf(stderr, "Feedback::ReceivedFrame(%s) got frame (%d,%d), %d channels, type %d\n", info, frame.rows, frame.cols, frame.channels(), frame.type());
  UIImage* image = matrixToImage(frame); // FIXME: Who owns that?
  [sViewController performSelectorOnMainThread:showFrameSelector withObject:image waitUntilDone:false];
  fprintf(stderr, "Feedback::ReceivedFrame(%s) dispatched frame\n", info);

  NSString* message = [[NSString alloc] initWithUTF8String:info];
  [sViewController performSelectorOnMainThread:showTextSelector withObject:message waitUntilDone:false];
  fprintf(stderr, "Feedback::ReceivedFrame(%s) dispatched label\n", [message UTF8String]);
}

void
Feedback::OperationComplete() {
  [sViewController performSelectorOnMainThread:operationCompleteSelector withObject:nullptr waitUntilDone:false];
}

void
Feedback::CameraSnap() {
  fprintf(stderr, "Feedback::CameraSnap(): FIXME: Implement\n");
}

void
Feedback::CannotTakePicture() {
  fprintf(stderr, "Feedback::CannotTakePicture(): FIXME: Implement\n");
}

void
Feedback::PlaySound(const std::string& aSoundPath) {
  lighthouse::Player::Play(aSoundPath);
}

void
Feedback::PlaySound(const std::string &aSoundPath, float aVolume) {
  lighthouse::Player::Play(aSoundPath, aVolume);
}
