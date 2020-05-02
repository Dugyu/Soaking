#ifndef COMMON_H
#define COMMON_H

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#ifdef __APPLE__
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "glsupport.h"
#include "material.h"
#include "uniforms.h"

extern std::shared_ptr<Material> g_overridingMaterial;

const bool g_Gl2Compatible = false;

// takes MVM and its normal matrix to the shaders
inline void sendModelViewNormalMatrix(Uniforms &uniforms, const Matrix4 &MVM,
                                      const Matrix4 &NMVM) {
    uniforms.put("uModelViewMatrix", MVM).put("uNormalMatrix", NMVM);
}
 
inline void checkGlewVersion(){

    // on Mac, we shouldn't use GLEW.
    #ifndef __APPLE__
            glewInit(); // load the OpenGL extensions
    #endif
            std::cout << (g_Gl2Compatible ? "Will use OpenGL 2.x / GLSL 1.0"
                                     : "Will use OpenGL 3.x / GLSL 1.5")
                 << std::endl;
    #ifndef __APPLE__
            if ((!g_Gl2Compatible) && !GLEW_VERSION_3_0)
                throw runtime_error("Error: card/driver does not support OpenGL "
                                    "Shading Language v1.3");
            else if (g_Gl2Compatible && !GLEW_VERSION_2_0)
                throw runtime_error("Error: card/driver does not support OpenGL "
                                    "Shading Language v1.0");
    #endif
}
#endif
