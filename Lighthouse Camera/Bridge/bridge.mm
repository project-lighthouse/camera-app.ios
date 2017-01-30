#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/ios.h>

#import "bridge.h"

#include "lighthouse.hpp"
#include "exceptions.hpp"
#include "filesystem.hpp"
#include "player.hpp"

#import <Foundation/NSError.h>

NSObject *sViewController;

@implementation Bridge

lighthouse::ImageMatchingSettings matchingSettings = {
  .mNumberOfFeatures = 1000,
  .mMinNumberOfFeatures = 50,
  .mMatchingScoreThreshold = 10.0,
  .mRatioTestK = 0.8,
  .mHistogramWeight = 5.0,
};

lighthouse::Lighthouse lighthouseInstance(matchingSettings);

- (UIImage *)DrawKeypoints:(UIImage *)aSource {
  cv::Mat outputMatrix;
  cv::Mat inputMatrix;

  UIImageToMat(aSource, inputMatrix);

  lighthouseInstance.DrawKeypoints(inputMatrix, outputMatrix);

  return MatToUIImage(outputMatrix);
}

- (void)PlayVoiceLabel:(NSString *)aId {
  lighthouseInstance.PlayVoiceLabel(lighthouseInstance.GetDescription([aId UTF8String]));
}

- (void)RecordVoiceLabel:(NSString *)aId {
  lighthouseInstance.RecordVoiceLabel(lighthouseInstance.GetDescription([aId UTF8String]));
}

- (void)PlaySound:(NSString *)aSoundResourceName {
  lighthouse::Player::Play(Filesystem::GetResourcePath([aSoundResourceName UTF8String], "wav", "sounds"));
}

// Selectors used for dynamic calls to delegate methods.
SEL onSuccess = @selector(onSuccess:);
SEL onError = @selector(onError:);
SEL onProgress = @selector(onProgress:);

/**
 * A wrapper for a `BasicDelegate` as a `lighthouse::BasicDelegate`.
 *
 * This wrapper takes care of wrapping Swift/Obj-C reference-counting in a C++ class,
 * as well as of performing calls on the main thread.
 */
class WrappedBasicDelegate: public lighthouse::BasicDelegate {
public:
  WrappedBasicDelegate(LighthouseDelegate* aDelegate):
    mWrapped(aDelegate) { }
  virtual ~WrappedBasicDelegate() {
    mWrapped = nil; // Not entirely sure that's needed. Semantics of Obj-C++ are always... interesting.
  }
  virtual void OnProgress(uint32_t aProgress) override {
    fprintf(stderr, "WrappedBasicDelegate::OnProgress(%u) (%lu bytes vs. %lu bytes)\n", aProgress, sizeof(aProgress), sizeof(unsigned long));
    [mWrapped performSelectorOnMainThread:onProgress withObject:[[NSNumber alloc] initWithUnsignedInt:aProgress] waitUntilDone:false];
  }
  virtual void OnError(LighthouseError aError) override {
    [mWrapped performSelectorOnMainThread:onError withObject:[NSError errorWithDomain:@"org.mozilla.cd.ios.lighthouse" code:aError userInfo:nil] waitUntilDone:false];
  }
protected:
  // The `__strong` annotation should ensure that class `WrappedImageDelegate`
  // maintains a refcounted pointer towards `mWrapped` until it is destroyed.
  // FIXME: How do we unit-test this?
  __strong LighthouseDelegate* mWrapped;
};

/**
 * A wrapper for a `LighthouseUIImageDelegate` as a `lighthouse::ImageDelegate`.
 *
 * This wrapper takes care of wrapping Swift/Obj-C reference-counting in a C++ class,
 * as well as of performing calls on the main thread.
 */
class WrappedImageDelegate: public WrappedBasicDelegate, public lighthouse::ImageDelegate {
public:
  WrappedImageDelegate(LighthouseUIImageDelegate* aDelegate):
    WrappedBasicDelegate(aDelegate) { }
  virtual ~WrappedImageDelegate() {
    mWrapped = nil; // Not entirely sure that's needed. Semantics of Obj-C++ are always... interesting.
  }
public:
  virtual void OnSuccess(cv::Mat aImage) override {
    UIImage* image = MatToUIImage(aImage);
    [mWrapped performSelectorOnMainThread:onSuccess withObject:image waitUntilDone:false];
  }
  virtual void OnProgress(uint32_t aProgress) override {
    WrappedBasicDelegate::OnProgress(aProgress);
  }
  virtual void OnError(LighthouseError aError) override {
    WrappedBasicDelegate::OnError(aError);
  }
};

class WrappedValueDelegate: public WrappedBasicDelegate, public lighthouse::ValueDelegate {
public:
  WrappedValueDelegate(LighthouseDelegate* aDelegate, uint32_t aValue):
    WrappedBasicDelegate(aDelegate),
    lighthouse::ValueDelegate(aValue) {}
  virtual void OnProgress(uint32_t aProgress) override {
    fprintf(stderr, "WrappedValueDelegate::OnProgress(%u)\n", aProgress);
    WrappedBasicDelegate::OnProgress(aProgress);
  }
  virtual void OnError(LighthouseError aError) override {
    WrappedBasicDelegate::OnError(aError);
  }
};

- (void)doNoop:(LighthouseDelegate *)cb withResult:(uint32_t)result {
  std::unique_ptr<lighthouse::ValueDelegate> wrapped(new WrappedValueDelegate(cb, result));
  lighthouseInstance.DoCallback(std::move(wrapped));
}

- (void)doRecordObject: (LighthouseUIImageDelegate*)cb {
  std::unique_ptr<lighthouse::ImageDelegate> wrapped(new WrappedImageDelegate(cb));
  lighthouseInstance.DoRecordObject(std::move(wrapped));
}

- (void)onIdentifyObject {
  lighthouseInstance.OnIdentifyObject();
}

- (void)onStopCapture {
  fprintf(stderr, "onStopCapture %s", "start");
  lighthouseInstance.StopRecord();
  fprintf(stderr, "onStopCapture %s", "stop");
}
@end


@implementation LighthouseDelegate
- (void)onError:(NSError*)code {
#pragma unused (code)
  fprintf(stderr, "LighthouseDelegate::onError (default implementation)\n");
}
- (void)onProgress:(uint32_t)code {
#pragma unused (code)
  fprintf(stderr, "LighthouseDelegate::progress (default implementation)\n");
}
@end

@implementation LighthouseUIImageDelegate
- (void)onSuccess:(UIImage*)result {
#pragma unused (result)
  fprintf(stderr, "UIImageDelegate::onSuccess (default implementation)\n");
}
@end
