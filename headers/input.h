//
//  input.h
//  Soaking
//
//  Created by Guangyu Du on 4/27/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//

#ifndef input_h
#define input_h

#ifdef __APPLE__
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "cvec.h"
#include "quat.h"
#include "rigtform.h"
#include "window.h"
#include "scene.h"

struct Input
{
    int iClickX_;
    int iClickY_;
    bool iLClickButton_;
    bool iRClickButton_;
    bool iMClickButton_;
    bool iClickDown_ = false;
    bool iSpaceDown_ = false;

//    enum ManipMode {
//      ARCBALL_ON_PICKED,
//      ARCBALL_ON_SKY,
//      EGO_MOTION
//    };
    
    //   0 ARCBALL_ON_PICKED,
    //   1 ARCBALL_ON_SKY,
    //   2 EGO_MOTION

//    enum ViewManipMode {
//      ROTATION,
//      TRANSLATION_XY,
//      TRANSLATION_Z
//    };

    //  0 ROTATION,
    //  1 TRANSLATION_XY,
    //  2 TRANSLATION_Z

    std::shared_ptr<Window> window_;
    std::shared_ptr<Scene> scene_;

    Input(){};
    void setWindow(std::shared_ptr<Window> window){
        window_ = window;
    }
    void setScene(std::shared_ptr<Scene> scene){
        scene_ = scene;
    }
    
    void updateClickInput(const int &button, const int &state, const int &x, const int &y){
        iClickX_ = x;
        iClickY_ = window_->windowHeight_ - y - 1;// conversion from GLUT window-coordinate-system
        // to OpenGL window-coordinate-system

        iLClickButton_ |= (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);
        iRClickButton_ |= (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN);
        iMClickButton_ |= (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN);

        iLClickButton_ &= !(button == GLUT_LEFT_BUTTON && state == GLUT_UP);
        iRClickButton_ &= !(button == GLUT_RIGHT_BUTTON && state == GLUT_UP);
        iMClickButton_ &= !(button == GLUT_MIDDLE_BUTTON && state == GLUT_UP);

        iClickDown_ = iLClickButton_ ||iRClickButton_ || iMClickButton_;
        
    }
    
    bool onMotion(const int &x, const int &y)
    {
        if (!iClickDown_) return false;
        //    if (g_activeObject == SKY && g_activeEye != SKY) return;
        //    // we do not edit the sky when viewed from the objects
        const double dx = x - iClickX_;
        const double dy = window_->windowHeight_- y - 1 - iClickY_;

        const RigTForm M = getMotionRbt(dx, dy);
        const RigTForm My = getMotionRbt(dx, 0);
        const RigTForm Mx = getMotionRbt(0, dy);
        
        scene_->onMotionCallBack(M, isRotating(), Mx, My);
        
        iClickX_ += dx;
        iClickY_ += dy;
        
        return true;
    }
    
    int getViewManipMode(){
        if (iLClickButton_ && !iRClickButton_ && !iSpaceDown_) {
            return 0;  // ROTATION
        }else if (iRClickButton_ && !iLClickButton_){
            return 1;  // TRANSLATION_XY
        }else if(iMClickButton_ || (iLClickButton_ && iRClickButton_) || (iLClickButton_ && iSpaceDown_)){
            return 2;  // TRANSLATION_Z
        }
        return 0;
    }
    
    bool isRotating(){
       return (getViewManipMode() == 0);
    }
    

    double getMotionScale(){
        return scene_->getMotionScale();
    }
    
    RigTForm getMotionRbt(const double & dx, const double & dy){
        RigTForm M;
        switch (getViewManipMode()) {
            case 0:
                M = RigTForm(Quat::makeXRotation(-dy) * Quat::makeYRotation(dx));
                break;
            case 1:
                M = RigTForm(Cvec3(dx, dy, 0) * getMotionScale());
                break;
            case 2:
                M = RigTForm(Cvec3(0, 0, -dy) * getMotionScale());
                break;
             default:
                break;
        }

        switch (scene_->getManipMode()) {
        case 0:
           break;
        case 1:
           M = inv(M);
           break;
        case 2:
           M = inv(M);
           break;
        default:
           break;
        }

         return M;

    }
    
    
};
#endif /* input_h */
