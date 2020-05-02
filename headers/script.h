//
//  script.h
//  cs175-asst7
//
//  Created by Guangyu Du on 4/10/20.
//  Copyright © 2020 cs175. All rights reserved.
//

#ifndef script_h
#define script_h

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

#include "sgutils.h"
#include "frame.h"

struct Script{
    // for keyframe generation
    std::list<Frame> sceneFrames_;
    std::list<Frame>::iterator curFrame_;
    int frameIndex_; // at the same pos with curFrame_, fast access index
    
    // for animation
    std::list<Frame>::iterator prevDisplayFrame_;
    std::list<Frame>::iterator curDisplayFrame_;
    std::list<Frame>::iterator nextDisplayFrame_;
    std::list<Frame>::iterator futureDisplayFrame_;
    int frameBetweenCount_;
    int curFrameBetweenIndex_;
    bool animationPlaying_;

    Script(){
        curFrame_ = sceneFrames_.begin();
        frameIndex_ = 0;
        frameBetweenCount_ = 0;
        curFrameBetweenIndex_ = 0;
        animationPlaying_ = false;
    };
    
    Script (shared_ptr<SgNode> root, std::string filename = "animation_in.txt"){
        curFrame_ = sceneFrames_.begin();
        frameIndex_ = 0;
        frameBetweenCount_ = 0;
        curFrameBetweenIndex_ = 0;
        animationPlaying_ = false;
        
        std::ifstream file;
        file.open(filename.c_str()); // turn string into const char *
        vector<std::string> lines = vector<std::string>();
        vector<int> stats = vector<int>();

        if (!file) {
            std::cout<<"No file named "<<filename<<"."<<endl;
        }

        std::string l;
        int count = 0;
        while (std::getline(file, l)){
            if (count < 4){
                if (count == 1 or count == 3) stats.push_back(std::stoi(l));
            }else{
                lines.push_back(l);
            }
            count ++;
        }
        file.close();
        
        //Construct Script;
        vector<std::string> frameLines = vector<std::string>();
        for (int i = 0; i<lines.size(); i++){
            frameLines.push_back(lines[i]);
            if (frameLines.size() == stats[1]){
                Frame frame;
                frame.setSceneRoot(root);
                frame.setRbtsFromFile(frameLines);
                sceneFrames_.push_back(frame);
                std::cout<<"size "<<sceneFrames_.size()<<std::endl;
                // clean buffer
                frameLines = vector<std::string>();
            }
        }
        
        std::cout<<"Stats "<<stats[0]<<" "<<stats[1]<<std::endl;
    }
    
    void outputKeyFrameToFile (std::string filename = "animation_out.txt"){
        std::ofstream file;
        file.open(filename.c_str()); // turn string into const char *
        std::string s;

        if (sceneFrames_.size()<1){
            s = "No Animated Frames";
            
        }else{
            s = "FrameCount\n" + std::to_string(sceneFrames_.size()) + "\n" + "SceneNode\n" + std::to_string(sceneFrames_.begin()->nodeCount()) + "\n";
            
            for (std::list<Frame>::iterator it = sceneFrames_.begin(); it != sceneFrames_.end(); it++){
                s += it->outputFrameStats();
            }
        }
        file << s;
        file.close();
    }
    
    int frameCount(){
        return (int)sceneFrames_.size();
    }
    int currentFrameIndex(){
        return frameIndex_;
    }
    
    bool animationEnded(){
        return !animationPlaying_;
    }
    
    void addKeyFrameToEnd(Frame frame){
       sceneFrames_.push_back(frame);
    }
    
    void applyCurrentFrameToScene(){
        if (!sceneFrames_.empty()){
            curFrame_->setSceneRbt();
        }
    }
    
    void updateCurrentKeyFrame(Frame frame){
        if (sceneFrames_.empty() || curFrame_ == sceneFrames_.end()){
            insertKeyFrameCurrentAfter(frame);
        }else{
            * curFrame_ = frame;
        }
    }
    
    void insertKeyFrameCurrentAfter(Frame frame){
        if (sceneFrames_.empty()){
            // empty frame list
            sceneFrames_.push_back(frame);
            curFrame_ = sceneFrames_.begin();
        }else{
            // not empty frame list
            curFrame_ ++;
            frameIndex_ ++;
            curFrame_ = sceneFrames_.insert(curFrame_, frame);
        }
    }
    
    void deleteCurrentKeyFrame(){
        if (sceneFrames_.empty()) return;
        
        if (curFrame_ == sceneFrames_.end()) {
            curFrame_ = sceneFrames_.begin();
            frameIndex_ = 0;
        }
        
        curFrame_ = sceneFrames_.erase(curFrame_);
        if (curFrame_ == sceneFrames_.end())
        {
            curFrame_ = sceneFrames_.begin();
            frameIndex_ = 0 ;
        }
        curFrame_->setSceneRbt();
    }
    
    void retreatKeyFrame(){
        if (sceneFrames_.empty()) return;
        
        curFrame_ --;
        frameIndex_ --;
        
        // reach the very begining
        if (frameIndex_ < 0) {
            curFrame_ = sceneFrames_.end(); // reset to the end
            curFrame_ --;
            frameIndex_ = (int)sceneFrames_.size() - 1;
        }
        curFrame_->setSceneRbt();
    }
    
    void advanceKeyFrame(){
        if (sceneFrames_.empty()) return;
        
        curFrame_ ++;
        frameIndex_ ++;
        
        // reach the very end
        if (curFrame_ == sceneFrames_.end()){
            curFrame_ = sceneFrames_.begin();
            frameIndex_ = 0;
        }
        curFrame_->setSceneRbt();
    }
    
    
    Frame nextFrame(){
        double alpha = (1.0 / (double)(frameBetweenCount_ + 1)) * curFrameBetweenIndex_;
    
       // if reaches the end of animation
        if (futureDisplayFrame_ == sceneFrames_.end()){
            animationPlaying_ = false;
            return *curDisplayFrame_;
        }else{
            curFrameBetweenIndex_ ++;
        }
        
        Frame interFrame = Frame::smoothInterpolate(*prevDisplayFrame_, *curDisplayFrame_, *nextDisplayFrame_, *futureDisplayFrame_, alpha);

        // jump to next key frame
        if (curFrameBetweenIndex_ > frameBetweenCount_) {
            curFrameBetweenIndex_ = 0;
            prevDisplayFrame_ ++;
            curDisplayFrame_ ++ ;
            nextDisplayFrame_ ++;
            futureDisplayFrame_ ++;
        }
        
        return interFrame;
    }
    
    
    bool startAnimation(int frameBetweenCount){
        // return true if animation starts successfully
        // else false
        if (sceneFrames_.size() < 4) return false; // insufficient frames
        
        frameBetweenCount_ = frameBetweenCount;
        if (frameBetweenCount < 0) frameBetweenCount_ = 0;
        
        // reset cusor for display frame
        // prev, cur, next, future
        // c_i-1, c_i, c_i+1, c_i+2
        curFrameBetweenIndex_ = 0;
        prevDisplayFrame_ = sceneFrames_.begin();  // the first frame
        curDisplayFrame_ = prevDisplayFrame_;
        curDisplayFrame_ ++;    // from the second frame
        nextDisplayFrame_ = curDisplayFrame_;
        nextDisplayFrame_ ++;    // from the third frame
        futureDisplayFrame_ = nextDisplayFrame_;
        futureDisplayFrame_ ++;   // from the fourth frame
        animationPlaying_ = true;
        return true;
    }
    
    void endAnimation(){
        animationPlaying_ = false;
    }
    
    void changeAnimationSpeed(int frameBetweenCount){
        double curAlpha = (double)curFrameBetweenIndex_ / (double)(frameBetweenCount_ + 1);
        int newFrameBetweenIndex = floor(curAlpha * (frameBetweenCount + 1));
        curFrameBetweenIndex_ = newFrameBetweenIndex;
        // update
        frameBetweenCount_ = frameBetweenCount;
    }
};


#endif /* script_h */
