//
//  lighthouse.cpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#include "lighthouse.hpp"
#include "filesystem.hpp"
#include "recorder.hpp"
#include "player.hpp"

namespace lighthouse {

Lighthouse::Lighthouse(ImageMatchingSettings aImageMatchingSettings)
        : mImageMatcher(ImageMatcher(aImageMatchingSettings)),
          mCamera(),
          mDescriptions(),
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
        mImageMatcher.AddToDB(ImageDescription::Load(descriptionFolderPath + "/description.bin"));
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

ImageDescription Lighthouse::GetDescription(const cv::Mat &aInputFrame) const {
    return mImageMatcher.GetDescription(aInputFrame);
}

void Lighthouse::SaveDescription(const ImageDescription &aDescription) {
    const std::string descriptionFolderPath = mDbFolderPath + aDescription.GetId();
    Filesystem::CreateDirectory(descriptionFolderPath);

    Player::Play(Filesystem::GetResourcePath("after-the-tone", "wav", "sounds"));

    Player::Play(Filesystem::GetResourcePath("beep", "wav", "sounds"));
    Recorder::Record(descriptionFolderPath + "/short-voice-label.aiff");
    Player::Play(Filesystem::GetResourcePath("beep", "wav", "sounds"));

    ImageDescription::Save(aDescription, descriptionFolderPath + "/description.bin");
    mImageMatcher.AddToDB(aDescription);

    Player::Play(Filesystem::GetResourcePath("registered", "wav", "sounds"));
}

void Lighthouse::PlayVoiceLabelForDescription(const ImageDescription &aDescription) {
    Player::Play(mDbFolderPath + aDescription.GetId() + "/short-voice-label.aiff");
}

std::vector<std::tuple<float, ImageDescription>> Lighthouse::Match(const cv::Mat &aInputFrame) const {
    return Match(GetDescription(aInputFrame));
}

std::vector<std::tuple<float, ImageDescription>> Lighthouse::Match(const ImageDescription &aDescription) const {
    return mImageMatcher.Match(aDescription);
}

void Lighthouse::OnRecordObject() {
    mCamera.CaptureForRecord();
}

} // namespace lighthouse
