//
//  errors.hpp
//  Lighthouse Camera
//
//  Created by David Teller on 31/01/17.
//  Copyright © 2017 Lighthouse. All rights reserved.
//

#ifndef lighthouse_errors_h
#define lighthouse_errors_h

enum LighthouseError {
  // The operation could not complete because of an error in video.{cpp, hpp} // FIXME: Make it more detailed.
  ERROR_COULD_NOT_CAPTURE = 0,
  
  // The operation could not complete because we didn't manage to extract a description.
  ERROR_COULD_NOT_EXTRACT_DESCRIPTION = 1,
};

#endif /* lighthouse_errors_h */
