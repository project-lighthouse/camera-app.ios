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

#include <TargetConditionals.h>

#include <stdint.h>
#include <atomic>
#include <chrono>
#include <thread>

using namespace lighthouse;
using namespace cv;

// Open the camera.
//
// If the code is executed on the simulator (which doesn't have a camera), this opens a bundled video
// instead.
Ptr<VideoCapture> OpenCamera() {
  fprintf(stderr, "OpenCamera() start\n");
  auto capture = Ptr<VideoCapture>(new VideoCapture());
    
#if TARGET_IPHONE_SIMULATOR
  // Simulator specific code
    
  const std::string resourceName("now-you-see-me");
  const std::string resourceType("mp4");
  std::string path = Filesystem::GetResourcePath(resourceName, resourceType);
  if (!capture->open(path)) {
    fprintf(stderr, "OpenCamera() could not open bundled video\n");
    return Ptr<VideoCapture>();
  }
    
#else // TARGET_IPHONE_SIMULATOR
  // Device specific code
    
  if (!capture->open(0)) {
    fprintf(stderr, "OpenCamera() could not open camera 0\n");
    return Ptr<VideoCapture>();
  }
    
#endif // TARGET_IPHONE_SIMULATOR

  return capture;
}

bool
TakePicture(VideoCapture* aCapture, Mat& aResult) {
  // FIXME: Turn ON the flashlight.

  // FIXME: We might want to take several images and keep the least blurry.
  if (!aCapture->read(aResult)) {
    Feedback::CannotTakePicture();
    return false;
  }
  Feedback::CameraSnap();
  Feedback::ReceivedFrame("TakePicture", aResult);

  // FIXME: Turn OFF the flashlight.
  return true;
}

bool
DownsampleAndBlur(const Mat& image, const float DOWNSAMPLE_FACTOR, const float BLUR, Mat&result)
{
  Mat downsampled((int)image.rows*DOWNSAMPLE_FACTOR,
                  (int)image.cols*DOWNSAMPLE_FACTOR,
                  (int)image.type());
  fprintf(stderr, "DownsampleAndBlur: (%d, %d)\n", image.rows, image.cols);
  cv::resize(image, downsampled, downsampled.size());

  fprintf(stderr, "DownsampleAndBlur after resizing: (%d, %d)\n", downsampled.rows, downsampled.cols);
  Mat blurred(downsampled.rows,
              downsampled.cols,
              downsampled.type());
  cv::GaussianBlur(downsampled, blurred, Size(), BLUR, BLUR);

  fprintf(stderr, "DownsampleAndBlur after blur: (%d, %d)\n", blurred.rows, blurred.cols);
  result = blurred;

  return true;
}

bool
GetImageDelta(const Mat& imageA, const Mat& imageB,
              const float DOWNSAMPLE_FACTOR, const float BLUR, const float MIN_SIZE,
              Mat& result) {
  fprintf(stderr, "GetImageDelta with DOWNSAMPLE_FACTOR %f, BLUR %f, MIN_SIZE %f\n", DOWNSAMPLE_FACTOR, BLUR, MIN_SIZE);
  // We operate on downsized images, both for performance and to minimize the impact
  // of small changes on the image.

  Mat smallerA;
  if (!DownsampleAndBlur(imageA, DOWNSAMPLE_FACTOR, BLUR, smallerA)) {
    return false;
  }
  Mat channelsA[3];
  cv::split(smallerA, channelsA);
  Feedback::ReceivedFrame("smallerA", smallerA);

  Mat smallerB;
  if (!DownsampleAndBlur(imageB, DOWNSAMPLE_FACTOR, BLUR, smallerB)) {
    return false;
  }
  Mat channelsB[3];
  cv::split(smallerB, channelsB);
  Feedback::ReceivedFrame("smallerB", smallerB);

  Mat channelsDiff[3];
  for (int i = 0; i < 3; ++i) {
    fprintf(stderr, "GetImageDelta channel %i\n", i);
    channelsDiff[i] = Mat(channelsA[i].rows, channelsA[i].cols, channelsA[i].type());
    cv::absdiff(channelsA[i], channelsB[i], channelsDiff[i]);
    Feedback::ReceivedFrame("channelA", channelsA[i]);
    Feedback::ReceivedFrame("channelB", channelsB[i]);
    Feedback::ReceivedFrame("channelDiff", channelsDiff[i]);
  }

  // Compute a noisy delta of the downsampled images.
  fprintf(stderr, "GetImageDelta => downsampledNoisyDelta\n");
  Mat downsampledNoisyDelta = channelsDiff[0] / 3 + channelsDiff[1] / 3 + channelsDiff[2] / 3;
//  Feedback::ReceivedFrame(downsampledNoisyDelta);

  // Convert noisy delta into a noisy mask.
  fprintf(stderr, "GetImageDelta => downsampledNoisyMask\n");
  Mat downsampledNoisyMask(downsampledNoisyDelta.rows, downsampledNoisyDelta.cols, downsampledNoisyDelta.type());
  cv::threshold(downsampledNoisyDelta, downsampledNoisyMask, 1, 255, THRESH_BINARY_INV | THRESH_OTSU);
//  Feedback::ReceivedFrame(downsampledNoisyMask);
  
  // Get rid of small components (i.e. noise).
  Mat downsampledCleanMask(downsampledNoisyDelta.rows, downsampledNoisyDelta.cols, downsampledNoisyDelta.type());
  std::vector<std::vector<Point>> contours;
  cv::findContours(downsampledNoisyMask, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

  auto surface = downsampledNoisyMask.rows * downsampledNoisyMask.cols;
  for (auto iter = contours.begin(); iter != contours.end(); ++iter) {
    const std::vector<Point>& contour = *iter;
    auto area = contourArea(contour);
    double fraction = (double)area / (double)surface;
    if (fraction < MIN_SIZE) {
      // This is a small contour, get rid of it.
      continue;
    }

    // Contour is large enough, let's make it part of the actual mask.
    std::vector<std::vector<Point>> hulls(1);
    cv::convexHull(contour, hulls[0]);
    cv::fillPoly(downsampledCleanMask, hulls, 255);
  }

  // Now upsample back the mask and return it.
  cv::resize(downsampledCleanMask, result, result.size()); // FIXME: Pick a good resizer.
  return true;
}


// Image acquisition strategy.
//
// 1. Take a picture, assume it's the full image, including the object.
// 2. Wait.
// 3. Capture a second image, assume it's the image without the object.
// 4. Get rid of camera movement.
// 5. Compute the difference between both images, use it to remove the background.
template< class Rep, class Period >
bool
NowYouSeeMeNowYouDont(const std::chrono::duration<Rep, Period>& sleepDuration, const std::atomic_int *aState, const Task aTask, Mat& aResult) {
  auto capture = OpenCamera();

  // FIXME: Add feedback.

  fprintf(stderr, "NowYouSeeMeNowYouDont: Taking imageWithObject\n");
  Mat imageWithObject;
  if (!TakePicture(capture.get(), imageWithObject)) {
    Feedback::CannotTakePicture();
    return false;
  }


#if TARGET_IPHONE_SIMULATOR
  // We need to advance the video, otherwise we'll never receive the frames.
  int frames = 0;
  const std::chrono::duration<double, std::milli> fps(16); // For testing, we expect that 1 frame == 16ms.
  while (true) {
    fprintf(stderr, "NowYouSeeMeNowYouDont: waiting...\n");
    if (aState->load() != (int)aTask) {
      // We have been asked to stop. Bailout asap.
      return false;
    }
    Mat frame;
    if (!capture->read(frame)) {
      // Nothing more to capture.
      fprintf(stderr, "NowYouSeeMeNowYouDont: no more video...\n");
      return false;
    }
    ++frames;
    if (frames * fps >= sleepDuration) {
      break;
    }
  }

#else
  fprintf(stderr, "NowYouSeeMeNowYouDont: Waiting %f ms\n", std::chrono::duration<double, std::milli>(sleepDuration).count());
  auto start = std::chrono::high_resolution_clock::now();
  std::this_thread::sleep_for(sleepDuration);
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> elapsed = end-start;
  fprintf(stderr, "NowYouSeeMeNowYouDont: Waited %f ms\n", elapsed.count());

  // FIXME: Add feedback.

  if (aState->load() != (int)aTask) {
    // We have been interrupted.
    return false;
  }

#endif // TARGET_IPHONE_SIMULATOR

  fprintf(stderr, "NowYouSeeMeNowYouDont: Taking imageBackground\n");
  Mat imageBackground;
  if (!TakePicture(capture.get(), imageBackground)) {
    Feedback::CannotTakePicture();
    return false;
  }

  // FIXME: Add feedback.

  // FIXME: Compute camera movement between images using Phase Correlation.
  // FIXME: Get rid of camera movement.

  // Compute delta, extract object.
  fprintf(stderr, "NowYouSeeMeNowYouDont: Computing delta\n");
  const float DOWNSAMPLE_FACTOR = .5f;
  const double BLUR = .5;
  const double MIN_SIZE = .05;
  Mat imageMask(imageWithObject.rows, imageWithObject.cols, imageWithObject.type());
  if (!GetImageDelta(imageWithObject, imageBackground, DOWNSAMPLE_FACTOR, BLUR, MIN_SIZE, imageMask)) {
    return false;
  }

  if (aState->load() != (int)aTask) {
    // We have been interrupted.
    return false;
  }

#if DISPLAY_MASK
  fprintf(stderr, "NowYouSeeMeNowYouDont: Extracting object\n");
  Mat channels[4]; // BGRA
  cv::split(imageWithObject, channels);
  channels[3] = imageMask;

  fprintf(stderr, "NowYouSeeMeNowYouDont: Returning merged image\n");
  Mat result(imageWithObject.rows, imageWithObject.cols, imageWithObject.type());
  cv::merge(channels, 4, result);
  aResult = result;
#else
  fprintf(stderr, "NowYouSeeMeNowYouDont: Returning mask\n");
  Mat channels[3];
  channels[0] = imageMask;
  channels[1] = cv::Mat(imageMask.rows, imageMask.cols, imageMask.type());
  channels[2] = cv::Mat(imageMask.rows, imageMask.cols, imageMask.type());
  Mat result(imageWithObject.rows, imageWithObject.cols, imageWithObject.type());
  cv::merge(channels, 3, result);
  
  aResult = result;
#endif // 0

  fprintf(stderr, "NowYouSeeMeNowYouDont: Done\n");
  return true;
}

lighthouse::Camera::Camera()
{ }

void
Camera::CaptureForIdentification(std::atomic_int *aState) {
  // FIXME: TODO
}

void
Camera::CaptureForRecord(std::atomic_int *aState) {
  Mat frame;
  if (!NowYouSeeMeNowYouDont(std::chrono::milliseconds(1000), aState, Task::RECORD, frame)) {
    Feedback::OperationComplete();
    return;
  }
//  Feedback::ReceivedFrame(frame);
  Feedback::OperationComplete();


#if DEMO_BACKGROUND_SUBTRACTOR // FIXME: We can probably just get rid of this.
  auto capture = OpenCamera();
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
  fprintf(stderr, "CaptureForRecord() stop\n");
#endif // 0
}
