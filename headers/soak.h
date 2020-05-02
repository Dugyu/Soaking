//
//  soak.h
//  Soaking
//
//  Created by Guangyu Du on 4/26/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//

#ifndef soak_h
#define soak_h

#define GL_SILENCE_DEPRECATION

#include <iostream>

#ifdef __APPLE__
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include "common.h"
#include "cvec.h"
#include "rigtform.h"
#include "glsupport.h"
#include "uniforms.h"
#include "window.h"
#include "renderer.h"
#include "camera.h"
#include "assetlibrary.h"
#include "drawer.h"
#include "picker.h"
#include "input.h"
#include "scene.h"
#include "framebuffer.h"

std::shared_ptr<Material> g_overridingMaterial;

// Global App
struct Soak
{
    
    std::shared_ptr<Window> window_;
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<AssetLibrary> assetLibrary_;
    std::shared_ptr<Scene> scene_;
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<Input>  input_;
    
    std::shared_ptr<TextureFBO> prevFBO_;
    std::shared_ptr<TextureFBO> currentFBO_;
    std::shared_ptr<TextureFBO> nextFBO_;
          
    bool prevBufferInitialized = false;
    
    int timeSinceStart_ = 0;
    
    Soak(){
        window_.reset(new Window());
        renderer_.reset(new Renderer());
        assetLibrary_.reset(new AssetLibrary());
        scene_.reset(new Scene());
        camera_.reset(new Camera());
        input_.reset(new Input());
        
        input_->setWindow(window_);
        input_->setScene(scene_);
        camera_->setWindow(window_);
        scene_->setAssetLib(assetLibrary_);
     
    };

    // takes a projection matrix and send to the the shaders
    inline void sendProjectionMatrix(Uniforms& uniforms, const Matrix4& projMatrix) {
      uniforms.put("uProjMatrix", projMatrix);
    }
        
    
    void drawStuff(){

        // Declare an empty uniforms
        Uniforms uniforms;
        
        // only consider change in a minute
        int uTimerInMinute = timeSinceStart_ % 60000;
        
        uniforms.put("uTimer", uTimerInMinute);
          
        // build & send projection matrix to vshader
        const Matrix4 projmat = camera_->makeProjectionMatrix();
        sendProjectionMatrix(uniforms, projmat);
        
        const RigTForm eyeRbt = scene_->getEyeRbt();
        const RigTForm invEyeRbt = inv(eyeRbt);

        Drawer drawer(invEyeRbt, uniforms);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        scene_->nodeWorld_->accept(drawer);

        // first round
        // render real scene to Current Frame Texture

        currentFBO_->bind();
        glEnable(GL_DEPTH_TEST);
        renderer_->resetClearColor();
        // clear framebuffer color & depth
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene_->nodeWorld_->accept(drawer);

        // second round
        // composite Current Frame and Previous Frame to Next Frame Texture
        nextFBO_->bind();
        glDisable(GL_DEPTH_TEST);// screen space quad is not discarded
        renderer_->resetClearColor();
        glClear(GL_COLOR_BUFFER_BIT);
        if(!prevBufferInitialized){
            assetLibrary_->matScreen_->getUniforms().put("uDisplay", 0);
            prevBufferInitialized = true;
        }else{
            assetLibrary_->matScreen_->getUniforms().put("uDisplay", 1);
        }

        assetLibrary_->matScreen_->getUniforms().put("uTexCurFrame", currentFBO_->getTexture());
        assetLibrary_->matScreen_->getUniforms().put("uTexPrevFrame", prevFBO_->getTexture());
        assetLibrary_->matScreen_->getUniforms().put("uTexNextFrame", nextFBO_->getTexture());
        scene_->nodeScreenWorld_->accept(drawer);

        // third round
        // let screen use Next Frame Texture
        // bind to default screen buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        renderer_->resetClearColor(); // set to default;
        glClear(GL_COLOR_BUFFER_BIT);
        assetLibrary_->matScreen_->getUniforms().put("uDisplay", 2);
        scene_->nodeScreenWorld_->accept(drawer);

        // swich buffers
        prevFBO_ = nextFBO_;
        nextFBO_.reset(new TextureFBO(2*window_->windowWidth_, 2*window_->windowHeight_));
        currentFBO_.reset(new TextureFBO(2*window_->windowWidth_, 2*window_->windowHeight_));
    }

    void updateClickInput(const int &button, const int &state, const int &x, const int &y){
        input_->updateClickInput(button, state, x, y);
    }
    
    void initFrameBuffers(){
         //texture framebuffers;
         prevFBO_.reset(new TextureFBO( 2*window_->windowWidth_, 2*window_->windowHeight_));
         currentFBO_.reset(new TextureFBO( 2*window_->windowWidth_, 2*window_->windowHeight_));
         nextFBO_.reset(new TextureFBO( 2*window_->windowWidth_, 2*window_->windowHeight_));
    }
    
    void initWindow(){
        window_->initWindow();
        window_->checkMacDisplayBug();
    }
    
    void reshapeWindow(const int w, const int h){
        window_->resetWindowSize(w,h).reshapeWindow();
        renderer_->resizeViewPort(w,h);
          currentFBO_.reset(new TextureFBO(2 * window_->windowWidth_, 2 * window_->windowHeight_));
          nextFBO_.reset(new TextureFBO(2 * window_->windowWidth_,2 *  window_->windowHeight_));
    }
    
    void initGLState() {
        renderer_->initGLState();
    }
    
    void initMaterials(){
        assetLibrary_->initMaterials();
    }
    
    void initGeometry(){
        assetLibrary_->initGeometry();
    }
    
    void initScene(){
        scene_->initScene();
    }
    
    void updateFrustFovY(){
        camera_->updateFrustFovY();
    }
    
    bool onMotion(const int x, const int y){
        return input_->onMotion(x, y);
    }
    
    void updateUniforms(int timeSinceStart){
        timeSinceStart_ = timeSinceStart;
    }

    
};

#endif /* soak_h */
