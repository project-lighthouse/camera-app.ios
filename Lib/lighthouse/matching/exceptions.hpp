//
//  exceptions.hpp
//  Lighthouse Camera
//
//  Created by Aleh Zasypkin on 11/01/2017.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef exceptions_h
#define exceptions_h

#include <stdexcept>

namespace lighthouse {

// Describes all possible image quality related error codes.
enum ImageQualityExceptionCode {
  // Means that the number of keypoints we can extract from the image is less than minimum acceptable value.
  NotEnoughKeyPoints = 1
};

// Exception that is thrown whenever we have quality problems with the image during matching.
class ImageQualityException : public std::runtime_error {
public:
  ImageQualityException() : std::runtime_error("") {
  }

  ImageQualityException(const std::string &message, const ImageQualityExceptionCode aCode)
      : std::runtime_error(message) {
    mCode = aCode;
  }

  ImageQualityExceptionCode GetCode() const {
    return mCode;
  }

private:
  ImageQualityExceptionCode mCode;
};

} // namespace lighthouse

#endif /* exceptions_h */
