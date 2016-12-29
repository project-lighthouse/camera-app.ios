//
//  lighthouse.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "lighthouse.hpp"

namespace lighthouse {

Lighthouse::Lighthouse(int32_t aNumberOfFeatures): mImageMatcher(ImageMatcher(aNumberOfFeatures)) {}

void Lighthouse::DrawKeypoints(const cv::Mat &aInputFrame, cv::Mat &aOutputFrame) {
    ImageDescription description = mImageMatcher.GetDescription(aInputFrame);

    // We can't draw keypoints on the BGRA image.
    cv::Mat bgrInputFrame;
    cvtColor(aInputFrame, bgrInputFrame, cv::COLOR_BGRA2BGR);

    drawKeypoints(bgrInputFrame, description.GetKeypoints(), aOutputFrame, cv::Scalar::all(-1),
            cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
}

ImageDescription Lighthouse::GetDescription(const cv::Mat &aInputFrame) {
    return mImageMatcher.GetDescription(aInputFrame);
}

void Lighthouse::SaveDescription(const ImageDescription &aDescription, const std::string aPath) {
    ImageDescription::Save(aDescription, aPath);
}

const ImageDescription Lighthouse::LoadDescription(const std::string aPath) {
    return ImageDescription::Load(aPath);
}

} // namespace lighthouse