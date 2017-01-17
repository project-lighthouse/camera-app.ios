//
//  bridge.hpp
//  Lighthouse Camera
//
//  Created by David Teller on 06/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef bridge_hpp
#define bridge_hpp

#import <opencv2/opencv.hpp>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

cv::Mat imageToMatrix(UIImage *image);

/// Convert a BGR(A) OpenCV image to a UIImage.
UIImage* matrixToImage(const cv::Mat& matrix, UIImageOrientation* imageOrientation = nullptr);

#endif /* bridge_hpp */
