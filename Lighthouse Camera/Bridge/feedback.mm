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
#include "filesystem.hpp"
#include "player.hpp"
#include "bridge.h"


#import <Foundation/Foundation.h>

SEL showFrameSelector = @selector(showFrame:);
SEL showTextSelector = @selector(showText:);
SEL operationCompleteSelector = @selector(operationComplete);
SEL onItemRecordedSelector = @selector(onItemRecorded:);


void
Feedback::ReceivedFrame(const char* info, cv::Mat& frame) {
  fprintf(stderr, "Feedback::ReceivedFrame(%s) got frame (%d,%d), %d channels, type %d\n", info, frame.rows, frame.cols, frame.channels(), frame.type());
  UIImage* image = matrixToImage(frame); // FIXME: Who owns that?
  [sViewController performSelectorOnMainThread:showFrameSelector withObject:image waitUntilDone:false];
  fprintf(stderr, "Feedback::ReceivedFrame(%s) dispatched frame\n", info);
  ShowLabel(info);
}

void Feedback::ShowLabel(const char *info) {
  NSString* message = [[NSString alloc] initWithUTF8String:info];
  [sViewController performSelectorOnMainThread:showTextSelector withObject:message waitUntilDone:false];
  fprintf(stderr, "Feedback::ShowLabel(%s) dispatched label\n", [message UTF8String]);
}

void
Feedback::OperationComplete() {
  [sViewController performSelectorOnMainThread:operationCompleteSelector withObject:nullptr waitUntilDone:false];
}

void Feedback::OnItemRecorded(const std::string &aItemId) {
  NSString* itemId = [[NSString alloc] initWithUTF8String:aItemId.c_str()];
  [sViewController performSelectorOnMainThread:onItemRecordedSelector withObject:itemId waitUntilDone:false];
  fprintf(stderr, "Feedback::OnItemRecorded(%s) dispatched new item id.\n", aItemId.c_str());
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
Feedback::PlaySoundNamed(const std::string& aName) {
  lighthouse::Player::Play(Filesystem::GetResourcePath(aName, "wav", "sounds"));
}

void
Feedback::PlaySound(const std::string& aSoundPath) {
  lighthouse::Player::Play(aSoundPath);
}

void
Feedback::PlaySound(const std::string &aSoundPath, float aVolume) {
  lighthouse::Player::Play(aSoundPath, aVolume);
}
