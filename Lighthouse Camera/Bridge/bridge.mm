#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs/ios.h>

#import "bridge.h"

#include "lighthouse.hpp"
#include "exceptions.hpp"
#include "filesystem.hpp"
#include "player.hpp"

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

- (void)onRecordObject {
  lighthouseInstance.OnRecordObject();
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


