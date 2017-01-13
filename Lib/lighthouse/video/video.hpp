//
//  video.hpp
//  Lighthouse Camera
//
//  Created by David Teller on 05/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef video_hpp
#define video_hpp

#include <stdio.h>
#include <atomic>

namespace cv {
  struct Mat;
}

namespace lighthouse {

class Camera {
public:
  Camera();

  // Capture a video stream for the purpose of recording a new object.
  bool CaptureForRecord(std::atomic_int* aState, cv::Mat& aResult);

  // Capture a video stream for the purpose of identifying an already-known object.
  bool CaptureForIdentification(std::atomic_int* aState, cv::Mat& aResult);

private:
  Camera(const Camera& rhs) = delete;
  Camera& operator=(const Camera& rhs ) = delete;
};

}

#endif /* video_hpp */
