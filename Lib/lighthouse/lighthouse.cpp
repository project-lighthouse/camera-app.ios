//
//  lighthouse.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "lighthouse.hpp"
#include "filesystem.hpp"

namespace lighthouse {

Lighthouse::Lighthouse(int32_t aNumberOfFeatures): mImageMatcher(ImageMatcher(aNumberOfFeatures)), mDescriptions(),
                                                   mDbFolderPath() {
    Filesystem filesystem;

    // Create Data directory if it doesn't exist.
    mDbFolderPath = filesystem.GetRoot() + "/Data/";
    filesystem.CreateDirectory(mDbFolderPath);

    // Iterate through all sub folders, every folder should contain the following files:
    // 1. description.bin - binary serialized image description (keypoints, descriptors, histogram etc.);
    // 2. frame.bin - binary serialized image matrix. Optional, can be disabled;
    // 3. short-audio.wav - short voice label;
    // 4. long-audio.wav - long voice label.
    std::vector<std::string> subFolders = filesystem.GetSubFolders(mDbFolderPath);
    for (std::string descriptionFolderPath : subFolders) {
        mDescriptions.push_back(ImageDescription::Load(descriptionFolderPath + "/description.bin"));
    }
}

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

void Lighthouse::SaveDescription(const ImageDescription &aDescription) {
    const std::string descriptionFolderPath = mDbFolderPath + aDescription.GetId();

    Filesystem filesystem;
    filesystem.CreateDirectory(descriptionFolderPath);

    ImageDescription::Save(aDescription, descriptionFolderPath + "/description.bin");

    mDescriptions.push_back(aDescription);
}

} // namespace lighthouse
