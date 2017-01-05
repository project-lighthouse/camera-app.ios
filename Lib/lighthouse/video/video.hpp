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
#include <thread>

namespace lighthouse {
    
class Camera {
public:
    Camera();
    void CaptureForRecord();
    void CaptureForIdentification();
private:
    static void RunCaptureForRecord();
    std::thread* mCaptureThread;
private:
    Camera(const Camera& rhs);
    Camera& operator=(const Camera& rhs );
};

}

#endif /* video_hpp */
