//
//  lighthouse.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "lighthouse.hpp"

namespace lighthouse {

Lighthouse::Lighthouse(int32_t aNumberOfFeatures): mImageMatcher(0) {
    this->mImageMatcher = ImageMatcher(aNumberOfFeatures);
}

void Lighthouse::DrawKeypoints(const cv::Mat &aInputFrame, cv::Mat &aOutputFrame) {
    ImageDescription description = this->mImageMatcher.GetDescription(aInputFrame);

    // We can't draw keypoints on the BGRA image.
    cv::Mat bgrInputFrame;
    cvtColor(aInputFrame, bgrInputFrame, cv::COLOR_BGRA2BGR);

    drawKeypoints(bgrInputFrame, description.GetKeypoints(), aOutputFrame, cv::Scalar::all(-1),
            cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
}

} // namespace lighthouse