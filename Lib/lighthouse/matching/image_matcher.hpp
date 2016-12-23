//
//  image_matcher.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#ifndef image_matcher_hpp
#define image_matcher_hpp

#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include "image_description.hpp"

namespace lighthouse {

class ImageMatcher {
public:
    ImageMatcher(int32_t aNumberOfFeatures);

    const ImageDescription GetDescription(const cv::Mat &aInputFrame);
    
private:
    cv::Ptr<cv::Feature2D> mKeypointDetector;
    cv::Ptr<cv::DescriptorMatcher> mMatcher;
};

} // namespace lighthouse

#endif /* image_matcher_hpp */
