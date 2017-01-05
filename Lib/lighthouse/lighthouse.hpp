//
//  lighthouse.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 19/12/2016.
//  Copyright © 2016 Lighthouse. All rights reserved.
//

#ifndef lighthouse_hpp
#define lighthouse_hpp

#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

#include "image_matcher.hpp"
#include "video.hpp"

namespace lighthouse {

class Lighthouse {
public:
    Lighthouse(ImageMatchingSettings aImageMatchingSettings);

    void DrawKeypoints(const cv::Mat &aInputFrame, cv::Mat &aOutputFrame);

    ImageDescription GetDescription(const cv::Mat &aInputFrame) const;

    void SaveDescription(const ImageDescription &aDescription);

    std::vector<std::tuple<float, ImageDescription>> Match(const cv::Mat &aInputFrame) const;

    std::vector<std::tuple<float, ImageDescription>> Match(const ImageDescription &aDescription) const;

    void OnRecordObject();

private:
    ImageMatcher mImageMatcher;
    Camera mCamera;
    std::vector<ImageDescription> mDescriptions;
    std::string mDbFolderPath;
};

} // namespace lighthouse

#endif /* lighthouse_hpp */
