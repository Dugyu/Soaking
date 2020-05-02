//
//  framebuffer.h
//  Soaking
//
//  Created by Guangyu Du on 4/30/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//

#ifndef framebuffer_h
#define framebuffer_h

#include "common.h"
#include "glsupport.h"
#include "texture.h"

class FramebufferObject{
    public:
    
    virtual void bind() const = 0;
    virtual ~FramebufferObject() {}

};


class TextureFBO : public FramebufferObject{

    public:

    TextureFBO(const int & _width, const int & _height){
        screen_width = _width;
        screen_height = _height;
        tex.reset(new BufferTexture());
    };
    
    std::shared_ptr<BufferTexture> & getTexture() {
        return tex;
    }
    void updateDimensions(const int & _width, const int & _height) {
        screen_width = _width;
        screen_height = _height;
    }
    virtual void bind() const {
        // texture
        // create a color attachment texture
        glBindTexture(GL_TEXTURE_2D, tex->getHandle());
        
        if (g_Gl2Compatible)
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        if (!g_Gl2Compatible)
            glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       
        // rbo
        // create a renderbuffer object for depth and stencil attachment(won't be used in sampling)
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width, screen_height);
            

        // attach the texture and renderbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        //attach texture to currently bound framebuffer object
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->getHandle(), 0);
        //attach depth and stencil rbo to currently bound framebuffer object
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        
        //verify that framebuffer is complete
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        }
        
        checkGlErrors();
    }
    
    protected:
        GlFramebufferObject fbo;
        std::shared_ptr<BufferTexture> tex;
        GlRenderbufferObject rbo;
        
        int screen_width, screen_height;
};
#endif /* framebuffer_h */
