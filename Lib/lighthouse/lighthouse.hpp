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

#include "errors.hpp"
#include "image_matcher.hpp"
#include "video.hpp"

namespace lighthouse {

// Instructions for the off main thread event loop.
enum class Task {
  // Nothing to do.
  WAIT = 0,

  // Stop the thread as soon as possible.
  SHUTDOWN = 1,
 
  // Record a new object.
  RECORD = 2,

  // Identify an object.
  IDENTIFY = 3,
};

// The base class for the contents of a message sent to the event loop.
class AbstractPayload {
public:
  virtual ~AbstractPayload() {}
};


// A payload designed to delegate in case of progress or error.
// Subclasses are generally more useful.
class BasicDelegate: public AbstractPayload {
public:
  virtual void OnError(LighthouseError) = 0;
  virtual void OnProgress(uint32_t) = 0;
  virtual ~BasicDelegate() {}
};

// A payload specialized to only provide a specific value.
// Used for testing.
class ValueDelegate: public BasicDelegate {
public:
  ValueDelegate(uint32_t aValue):
    mValue(aValue)
  {
    fprintf(stderr, "ValueDelegate(%u)\n", mValue);
  }
public:
  const uint32_t mValue;
};

  // A payload specialized to return images in case of success.
class ImageDelegate: public BasicDelegate {
public:
  virtual void OnSuccess(cv::Mat aImage) = 0;
  virtual ~ImageDelegate() {}
};

  
// Describes all possible assets that are related to the image description, but are managed separately.
enum ImageDescriptionAsset {
  // Main binary data for the image description (id, descriptors, keypoints, histogram).
  Data,
  // Image description voice label.
  VoiceLabel,
  // Source image from which image description has been extracted.
  SourceImage
};

class Lighthouse {
public:
  Lighthouse(ImageMatchingSettings aImageMatchingSettings);

  ~Lighthouse();

  void DrawKeypoints(const cv::Mat &aInputFrame, cv::Mat &aOutputFrame);

  ImageDescription GetDescription(const cv::Mat &aInputFrame) const;

  const ImageDescription &GetDescription(const std::string &aId) const;

  // Record voice label for the specified existing description.
  void RecordVoiceLabel(const ImageDescription &aDescription) const;

  // Play voice label for the specified existing description.
  void PlayVoiceLabel(const ImageDescription &aDescription);

  void SaveDescription(const ImageDescription &aDescription, const cv::Mat &aSourceImage);

  std::vector<std::tuple<float, ImageDescription>> FindMatches(const cv::Mat &aInputFrame) const;

  std::vector<std::tuple<float, ImageDescription>> FindMatches(const ImageDescription &aDescription) const;

  // Start recording a new object.
  // Takes ownership of `aDelegate`, deletes it immediately after calling it.
  // Note that the methods of `aDelegate` may be called on any thread.
  void DoRecordObject(std::unique_ptr<ImageDelegate>&& aDelegate);

  // Do nothing, just call me back once the current operation is over.
  // Designed mostly for testing.
  void DoCallback(std::unique_ptr<ValueDelegate>&& aDelegate);
  
  // Start identifying an existing object.
  void OnIdentifyObject();

  // Stop recording/identifying object.
  void StopRecord();

private:
  // Run the C++ event loop on thread `mVideoThread`.
  //
  // The event loop is NEVER taken down.
  static void AuxRunEventLoop(Lighthouse *);

  // Send a message. Takes ownership of `object`.
  void SendMessage(Task aMessage, std::unique_ptr<AbstractPayload>&& aPayload);

  // Actual implementation of the event loop. Runs in `mVideoThread`.
  void RunEventLoop();

  // Actual implementation of recording an object. Runs in `mVideoThread`.
  void RunRecordObject(std::unique_ptr<AbstractPayload>&& payload);

  // Actual implementation of identifying an object. Runs in `mVideoThread`.
  void RunIdentifyObject();

  // Actual implementation of `DoCallback`. Runs in `mVideoThrad`.
  void RunCallback(std::unique_ptr<AbstractPayload>&& aDelegate);

  // Returns a file name of the description asset (data, voice label, source image).
  std::string GetDescriptionAssetName(const ImageDescriptionAsset aAsset) const;

  // Builds a full absolute path the image description's asset based on description id and asset type.
  std::string GetDescriptionAssetPath(const std::string &aDescriptionId, const ImageDescriptionAsset aAsset) const;

  // A thread designed to run all blocking camera/vision operations.
  std::thread mVideoThread;
  // Id of the video thread. Use it only to check that you are on that thread.
  std::thread::id mVideoThreadId;

  // A stamp incremented each time we send a message to the event loop.
  // Protected by mTaskMutex.
  uint64_t mTaskStamp;
  // All messages. Protected by `mTaskMutex`.
  struct Message {
    Task task;
    std::unique_ptr<AbstractPayload> payload;
  };
  std::vector<Message> mPendingMessages;
  // The latest task on record. Used to interrupt ongoing tasks.
  std::atomic<int> mLatestTask;

  std::mutex mTaskMutex;
  // Condition variable used to communicate with mCaptureThread.
  // Protected by mTaskMutex.
  std::condition_variable mTaskCondition;
  // The camera. Access only on mVideoThread.
  Camera mCamera;

  ImageMatcher mImageMatcher;
  std::string mDbFolderPath;
};

} // namespace lighthouse

#endif /* lighthouse_hpp */
