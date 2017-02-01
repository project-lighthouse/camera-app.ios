//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include "errors.hpp"

/**
 * Static variable: the view controller.
 *
 * Exposed for the sake of `Feedback`, so as to provide visual/auditory feedback to the user.
 */
extern NSObject *sViewController;

/**
 * Base class for delegates passed to the Lighthouse lib.
 *
 * These delegates are guaranteed to be called on the main thread, regardless of the thread
 * that performed the computer vision operation.
 */
@interface LighthouseDelegate: NSObject
/**
 * Called in case of error (including if the operation was canceled).
 *
 * Error codes are defined in errors.hpp.
 */
- (void)onError:(NSError*)error;

/**
 * Called whenever a new step is reached.
 */
- (void)onProgress:(NSNumber*)code;
@end

/**
 * A delegate receiving an image.
 */
@interface LighthouseUIImageDelegate: LighthouseDelegate
 /**
  * Called if the operation succeeds.
  */
- (void)onSuccess:(UIImage* )result;
@end

@interface Bridge : NSObject
- (UIImage *)DrawKeypoints:(UIImage *)aSource;

// Play voice label for the item description with the specified id.
- (void)PlayVoiceLabel:(NSString *)aId;

// Record voice label for the item description with the specified id.
- (void)RecordVoiceLabel:(NSString *)aId;

- (void)PlaySound:(NSString *)aSoundResourceName;

// Trigger C++ code to start recording new object.
- (void)doRecordObject: (LighthouseUIImageDelegate*)cb;

// Trigger a no-op on the C++ event loop.
// Used for testing.
- (void)doNoop: (LighthouseDelegate*)cb withResult:(uint32_t)result;

// Trigger C++ code to start identifying an object.
- (void)onIdentifyObject;

// Trigger C++ code to stop an ongoing Record/Identify operation.
- (void)onStopCapture;
@end
