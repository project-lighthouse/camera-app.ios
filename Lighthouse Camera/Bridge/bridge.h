//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <Google/Analytics.h>

/**
 * Static variable: the view controller.
 *
 * Exposed for the sake of `Feedback`, so as to provide visual/auditory feedback to the user.
 */
extern NSObject *sViewController;

@interface Bridge : NSObject
- (UIImage *)DrawKeypoints:(UIImage *)aSource;

// Play voice label for the item description with the specified id.
- (void)PlayVoiceLabel:(NSString *)aId;

// Record voice label for the item description with the specified id.
- (void)RecordVoiceLabel:(NSString *)aId;

- (void)PlaySound:(NSString *)aSoundResourceName;

// Trigger C++ code to start recording new object.
- (void)onRecordObject;

// Trigger C++ code to start identifying an object.
- (void)onIdentifyObject;

// Trigger C++ code to stop an ongoing Record/Identify operation.
- (void)onStopCapture;
@end
