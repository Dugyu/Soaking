//
//  frame.h
//  cs175-asst7
//
//  Created by Guangyu Du on 4/10/20.
//  Copyright © 2020 cs175. All rights reserved.
//

#ifndef frame_h
#define frame_h

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>


#include "rigtform.h"
#include "sgutils.h"

struct Frame {
    std::vector<RigTForm> nodeRbts_;
    std::vector<std::shared_ptr<SgRbtNode> > nodes_;
    shared_ptr<SgNode> root_;
    Frame() {};
    
    void setSceneRoot(shared_ptr<SgNode> root){
        root_ = root;
        dumpSgRbtNodes(root_, nodes_);
        this->getSceneRbt();
    }
    
    void setSceneRbt(){
        for ( int i = 0; i < nodes_.size(); i++){
            nodes_[i]->setRbt(nodeRbts_[i]);
        }
    }
    void getSceneRbt(){
        if (nodeRbts_.empty()){
          for ( int i = 0; i < nodes_.size(); i++){
                nodeRbts_.push_back(nodes_[i]->getRbt());
            }
        }else{
          for ( int i = 0; i < nodes_.size(); i++){
            nodeRbts_[i] = nodes_[i]->getRbt();
          }
        }
    }
    
    static RigTForm getRbtFromStats(std::string stats){
        std::stringstream ss(stats);
        std::string buffer;
        std::vector<double> r;
        while (ss >> buffer){
            r.push_back(std::stod(buffer));
        }
        return RigTForm(Cvec3(r[0],r[1],r[2]), Quat(r[3],r[4],r[5],r[6]));
    }
    
    void setRbtsFromFile(vector<std::string> stats){
        for (int i = 0; i < stats.size(); i++){
            RigTForm rbt = Frame::getRbtFromStats(stats[i]);
            nodeRbts_[i] = rbt;
        }
    }
    
    int nodeCount() const{
        return (int)nodes_.size();
    }
    
    std::string outputFrameStats(){
        stringstream s;
        for (RigTForm rbt : nodeRbts_) {
            Cvec3 t = rbt.getTranslation();
            Quat r = rbt.getRotation();
          s << t[0] << " " << t[1] << " " << t[2] << " ";
          s << r[0] << " " << r[1] << " " << r[2] << " " << r[3] << "\n";
        }
        return s.str();
    }
    
    static Frame interpolate(const Frame & f0, const Frame & f1, const double alpha){
        cout << "Keyframe interpolate alpha " << alpha << endl;
        
        // interpolation shortcut
        if (alpha <= 0) return f0;
        if (alpha >= 1) return f1;
        
        if (f0.nodeCount() != f1.nodeCount()) return f0;
        
        Frame f;
        f.setSceneRoot(f0.root_);
        for (int i = 0; i < f0.nodes_.size(); i++){
            f.nodeRbts_[i] = RigTForm::lerp(f0.nodeRbts_[i],f1.nodeRbts_[i], alpha);
        }
        return f;
    }
    
    static Frame smoothInterpolate(const Frame & f00, const Frame & f0, const Frame & f1, const Frame & f11, const double alpha){
        cout << "Keyframe interpolate alpha " << alpha << endl;
        if (alpha <= 0) return f0;
        if (alpha >= 1) return f1;
        
        // check if four frames have the same node count
        if (f00.nodeCount() == f0.nodeCount() &&
            f00.nodeCount() == f1.nodeCount() &&
            f00.nodeCount() == f11.nodeCount()){
            
            Frame f;
            f.setSceneRoot(f0.root_);
            for (int i = 0; i < f0.nodeCount(); i++){
                f.nodeRbts_[i] = RigTForm::smoothLerp(f00.nodeRbts_[i], f0.nodeRbts_[i],f1.nodeRbts_[i],f11.nodeRbts_[i], alpha);
            }
            return f;
        }else{
            return f0;
        }
    }
    
};




#endif /* frame_h */
