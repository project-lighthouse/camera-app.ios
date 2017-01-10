//
//  lighthouse.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#ifndef lighthouse_hpp
#define lighthouse_hpp

#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include <stdatomic.h>
#include <atomic>
#include <condition_variable>


#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include "image_matcher.hpp"
#include "video.hpp"

namespace lighthouse {

enum class Task {
  // Nothing to do.
  WAIT = 0,

  // Record a new object.
  RECORD = 1,
  IDENTIFY = 2,
};

class Lighthouse {
public:
  Lighthouse(ImageMatchingSettings aImageMatchingSettings);

  ~Lighthouse();

  void DrawKeypoints(const cv::Mat &aInputFrame, cv::Mat &aOutputFrame);

  ImageDescription GetDescription(const cv::Mat &aInputFrame) const;

  const ImageDescription &GetDescription(const std::string &id) const;

  void SaveDescription(const ImageDescription &aDescription);

  void PlayVoiceLabelForDescription(const ImageDescription &aDescription);

  std::vector<std::tuple<float, ImageDescription>> Match(const cv::Mat &aInputFrame) const;

  std::vector<std::tuple<float, ImageDescription>> Match(const ImageDescription &aDescription) const;

  void OnRecordObject();

  void OnIdentifyObject();

  void StopRecord();

private:
  // Run the C++ event loop on thread `mVideoThread`.
  static void AuxRunEventLoop(Lighthouse *);

  void SendMessage(Task aMessage);

  void RunEventLoop();

  // A thread designed to run all blocking camera/vision operations.
  std::thread mVideoThread;
  // Representation of the latest `Task` requested from the event loop.
  std::atomic_int mTask;
  // A stamp incremented each time we send a message to the event loop.
  // Protected by mTaskMutex.
  uint64_t mTaskStamp;
  std::mutex mTaskMutex;
  // Condition variable used to communicate with mCaptureThread.
  std::condition_variable mTaskCondition;

  ImageMatcher mImageMatcher;
  Camera mCamera;
  std::vector<ImageDescription> mDescriptions;
  std::string mDbFolderPath;
};

} // namespace lighthouse

#endif /* lighthouse_hpp */
