//
//  scene.h
//  Soaking
//
//  Created by Guangyu Du on 4/27/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//

#ifndef scene_h
#define scene_h

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


#include "cvec.h"
#include "rigtform.h"
#include "material.h"
#include "common.h"
#include "geometry.h"
#include "scenegraph.h"
#include "drawer.h"
#include "picker.h"
#include "sgutils.h"
#include "assetlibrary.h"

typedef SgGeometryShapeNode MyShapeNode;

struct Scene{
    
    // SCENE NODE
    std::shared_ptr<SgRootNode> nodeWorld_;
    std::shared_ptr<SgRootNode> nodeScreenWorld_;
    std::shared_ptr<SgRbtNode> nodeSky_;
    std::shared_ptr<SgRbtNode> nodeCube_;
    std::shared_ptr<SgRbtNode> nodeParticleSystem_;
    std::shared_ptr<SgRbtNode> nodeScreen_;
    
    
    // SCENE STATUS POINTER
    std::shared_ptr<SgRbtNode> nodeActiveEye_;
    std::shared_ptr<SgRbtNode> nodeActiveObj_;

    std::shared_ptr<AssetLibrary> assetLibrary_;

    int manipMode_ = 1; // 0 ARCBALL_ON_PICKED, 1 ARCBALL_ON_SKY, 2 EGO_MOTION

    double arcballScale_ = 0.1;
    
    Scene(){};
    
    void setAssetLib(std::shared_ptr<AssetLibrary> assetLibrary){
        assetLibrary_ = assetLibrary;
    }
    
    void initScene(){
        nodeWorld_.reset(new SgRootNode());
        nodeSky_.reset(new SgRbtNode(RigTForm(Cvec3(0.0, 0.25, 4.0))));
        
        nodeCube_.reset(new SgRbtNode(RigTForm(Cvec3(2, 1, 0))));
        nodeCube_->addChild(std::shared_ptr<MyShapeNode>(
                                  new MyShapeNode(assetLibrary_->geoPrimitiveCube_, assetLibrary_->matDiffuseRed_)));
        
        nodeParticleSystem_.reset(new SgRbtNode(RigTForm(Cvec3(0, 0, 0))));
        nodeParticleSystem_->addChild(std::shared_ptr<MyShapeNode>(
                               new MyShapeNode(assetLibrary_->geoParticleSystem_, assetLibrary_->matParticleWhite_)));
        
        nodeActiveEye_ = nodeSky_;
        nodeActiveObj_ = nodeSky_;
             
        nodeWorld_->addChild(nodeSky_);
        //nodeWorld_->addChild(nodeCube_);
        nodeWorld_->addChild(nodeParticleSystem_);
        
        nodeScreenWorld_.reset(new SgRootNode());
        nodeScreen_.reset(new SgRbtNode(RigTForm(Cvec3(0, 0, 1))));
        nodeScreen_->addChild(std::shared_ptr<MyShapeNode>(
                                      new MyShapeNode(assetLibrary_->geoScreen_, assetLibrary_->matScreen_)));
        nodeScreenWorld_->addChild(nodeScreen_);
     
    }
    
    RigTForm getEyeRbt(){
        return getPathAccumRbt(nodeWorld_, nodeActiveEye_ , 0);
    }
    
    RigTForm doMtoOwrtA(const RigTForm& M, const RigTForm& O, const RigTForm& A) {
      return A * M * inv(A) * O;
    }
    
    RigTForm makeMixedFrame(const RigTForm& objRbt, const RigTForm& eyeRbt) {
      return transFact(objRbt) * linFact(eyeRbt);
    }
    
    void onMotionCallBack(const RigTForm & M, const bool isRotating, const RigTForm & Mx, const RigTForm & My){
        
        const RigTForm A = makeMixedFrame(getArcballRbt(), getPathAccumRbt(nodeWorld_, nodeActiveEye_));
        RigTForm O = doMtoOwrtA(M, getPathAccumRbt(nodeWorld_,nodeActiveObj_,0), A);
        
        if (isRotating && nodeActiveEye_ == nodeSky_) {
          RigTForm B = makeMixedFrame(getArcballRbt(), RigTForm());
          O = doMtoOwrtA(Mx, getPathAccumRbt(nodeWorld_, nodeActiveObj_,0), A);
          O = doMtoOwrtA(My, O, B);
        }
        nodeActiveObj_->setRbt(O);
    }
    
    int getManipMode(){
        return manipMode_;
    }
    
    
    double getMotionScale(){
        return getManipMode() == 2 ? 0.02 : arcballScale_;
    }
    
        
    RigTForm getArcballRbt() {
//  if(g_picked){
//    return getPathAccumRbt(g_world,g_currentPickedRbtNode);
//  }else{
    switch (getManipMode()) {
        case 0:
          return getPathAccumRbt(nodeWorld_, nodeActiveObj_);
          break;
        case 1:
          return RigTForm();
          break;
        case 2:
          return getPathAccumRbt(nodeWorld_, nodeActiveEye_);
          break;
        default:
          throw runtime_error("Invalid ManipMode");
          break;
        }
//     }
    }
    
};


#endif /* scene_h */
