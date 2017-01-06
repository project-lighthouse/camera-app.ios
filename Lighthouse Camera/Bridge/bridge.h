//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

/**
 * Static variable: the view controller.
 *
 * Exposed for the sake of `Feedback`, so as to provide visual/auditory feedback to the user.
 */
extern NSObject* sViewController;

@interface Bridge : NSObject
- (UIImage *)DrawKeypoints:(UIImage *)source;

- (void)SaveDescription:(UIImage *)source;

- (NSArray *)Match:(UIImage *)source;

- (void)onRecordObject;
@end
