//
//  window.h
//  Soaking
//
//  Created by Guangyu Du on 4/26/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//

#ifndef window_h
#define window_h

#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>
 
#ifdef __APPLE__
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "common.h"

struct Window
{
    
    int windowWidth_ = 720;
    int windowHeight_ = 400;
    bool macMoved_ = false;
    char const * windowTitle_ = "S O A K I N G";
    
    Window(){}
    
    void initWindow(){
        glutInitWindowSize(windowWidth_, windowHeight_); // create a window
        glutCreateWindow(windowTitle_);                  // title the window
    }
    
    Window & resetWindowSize(const int windowWidth, const int windowHeight){
        windowWidth_ = windowWidth;
        windowHeight_ = windowHeight;
        return *this;
    }
    void reshapeWindow(){
        glutReshapeWindow(windowWidth_, windowHeight_);
    }
    
    void checkMacDisplayBug(){
#ifdef __APPLE__
        if(!macMoved_) {
            windowWidth_ = windowWidth_ * 2 ;
            windowHeight_ = windowHeight_ * 2 ;
            reshapeWindow();
            macMoved_ = true;
        }
#endif
    }

};

#endif /* window_h */


