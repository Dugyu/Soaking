//
//  camera.h
//  Soaking
//
//  Created by Guangyu Du on 4/26/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//

#ifndef camera_h
#define camera_h

#include "cvec.h"
#include "matrix4.h"
#include "window.h"

struct Camera
{
    const float frustMinFov_ = 60.0; // A minimal of 60 degree field of view
    float frustFovY_ = frustMinFov_; // FOV in y direction (updated by updateFrustFovY)

    const float frustNear_ = -0.1;  // near plane
    const float frustFar_ = -50.0;  // far plane
    
    std::shared_ptr<Window> window_;
    
    Camera(){};
    
    void setWindow(std::shared_ptr<Window> window){
        window_ = window;
    }
    
    void updateFrustFovY() {
        if (window_->windowWidth_ >= window_->windowHeight_)
            frustFovY_ = frustMinFov_;
        else {
            const double RAD_PER_DEG = 0.5 * CS175_PI / 180;
            frustFovY_ = atan2(sin(frustMinFov_ * RAD_PER_DEG) * window_->windowHeight_ /
                                    window_->windowWidth_,
                                cos(frustMinFov_ * RAD_PER_DEG)) /
                          RAD_PER_DEG;
        }
    }
    
    Matrix4 makeProjectionMatrix() {
        return Matrix4::makeProjection(
            frustFovY_, window_->windowWidth_ / static_cast<double>(window_->windowHeight_),
            frustNear_, frustFar_);
    }

};

#endif /* camera_h */
