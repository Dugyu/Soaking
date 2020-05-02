//
//  renderer.h
//  Soaking
//
//  Created by Guangyu Du on 4/26/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//

#ifndef renderer_h
#define renderer_h



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


struct Renderer{
    //Cvec4 clearColor_ = Cvec4(240. / 255., 230. / 255., 230. / 255., 0.);
    Cvec4 clearColor_ = Cvec4(0.3,0.5,0.6,1.0);
    Renderer(){};
    
    void initGLState() {
        glClearColor(clearColor_[0], clearColor_[1], clearColor_[2], clearColor_[3]);
        glClearDepth(0.);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);
        glReadBuffer(GL_BACK);
        if (!g_Gl2Compatible)
            glEnable(GL_FRAMEBUFFER_SRGB);
    }
    
    void resizeViewPort(const int w, const int h){
        glViewport(0, 0, w, h);
    }
    
    void resetClearColor(Cvec4 clearColor = Cvec4(0.3,0.5,0.6,1.0)){
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }
    

};

#endif /* renderer_h */
