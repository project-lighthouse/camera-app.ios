//
//  errors.h
//  Pods
//
//  Created by David Teller on 31/01/17.
//
//

#ifndef errors_h
#define errors_h


enum class Error {
  // The operation could not complete because of an error in video.{cpp, hpp} // FIXME: Make it more detailed.
  ERROR_COULD_NOT_CAPTURE = 0,
  
  // The operation could not complete because we didn't manage to extract a description.
  ERROR_COULD_NOT_EXTRACT_DESCRIPTION = 1,
};


#endif /* errors_h */
