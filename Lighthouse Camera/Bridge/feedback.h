//
//  feedback.h
//  Lighthouse Camera
//
//  Created by David Teller on 06/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef feedback_h
#define feedback_h

@protocol FeedbackImpl
- (void) showFrame: (UIImage*) image;
- (void) showText: (NSString*)text;
- (void) operationComplete;

@end


#endif /* feedback_h */
