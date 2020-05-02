//
//  assetlibrary.h
//  Soaking
//
//  Created by Guangyu Du on 4/26/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//
#ifndef assetlibrary_h
#define assetlibrary_h

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdlib>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


#include "cvec.h"
#include "rigtform.h"
#include "texture.h"
#include "material.h"
#include "common.h"
#include "geometry.h"
#include "scenegraph.h"
#include "drawer.h"
#include "picker.h"
#include "sgutils.h"



float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

struct AssetLibrary
{
    // TEX
    std::shared_ptr<GeneralTexture> particleTexture_;
    
    // MAT
    std::shared_ptr<Material> matDiffuseRed_;
    std::shared_ptr<Material> matParticleWhite_;
    std::shared_ptr<Material> matScreen_;
    
    // GEO
    std::shared_ptr<Geometry> geoPrimitiveCube_;
    std::shared_ptr<SimpleIndexedGeometryPNCRX> geoParticleSystem_;
    std::shared_ptr<SimpleIndexedGeometryPNX> geoScreen_;
    
    AssetLibrary (){};
    
    void initMaterials(){

        Material particleMatPrototype("./shaders/particle-gl3.vshader", "./shaders/particle-gl3.fshader");
        
        particleTexture_.reset(new GeneralTexture("./textures/Particle02.png", false));
        particleTexture_->bind();
        particleMatPrototype.getUniforms().put("uTexBase", particleTexture_);
        
        particleTexture_.reset(new GeneralTexture("./textures/Particle01.png", false));
        particleTexture_->bind();
            
        particleMatPrototype.getUniforms().put("uTexNoise", particleTexture_);

        particleMatPrototype.getRenderStates()
               .blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) // set blending mode
        .enable(GL_BLEND)                                // enable blending
        .disable(GL_CULL_FACE);                          // disable culling

        
        matParticleWhite_.reset(new Material(particleMatPrototype));
        matParticleWhite_->getUniforms().put("uColor", Cvec3(220. / 255., 130. / 255., 147./255.));
    
        
        Material solid("./shaders/basic-gl3.vshader", "./shaders/solid-gl3.fshader");
        matDiffuseRed_.reset(new Material(solid));
        matDiffuseRed_->getUniforms().put("uColor", Cvec3(220. / 255., 130. / 255., 147./255.));
        
        Material screen("./shaders/screen-gl3.vshader", "./shaders/screen-gl3.fshader");
        matScreen_.reset(new Material(screen));
        matScreen_->getUniforms().put("uDisplay",0);
        matScreen_->getUniforms().put("uColor",Cvec4(0.3,0.5,0.6,1.0));
    }
    
    void initGeometry(){
        initScreen();
        initCube();
        initParticleSystem(10000,0.5);
    }
    
    ~AssetLibrary(){};

private:
    void initScreen(){
        vector<VertexPNX> vtx;
        vector<unsigned short> idx{0,2,3,0,1,2};
        
        vtx.reserve(4);
        
        float h = 1.0;
        Cvec3f o1 = Cvec3f(-h, -h, 0.0);
        Cvec3f o2 = Cvec3f(h, -h, 0.0);
        Cvec3f o3 = Cvec3f(h, h, 0.0);
        Cvec3f o4 = Cvec3f(-h, h, 0.0);
        Cvec3f n = Cvec3f(0.0,0.0,1.0);
        
        VertexPNX bottomLeft = VertexPNX(o1, n, Cvec2f(0.0,0.0));
        VertexPNX bottomRight = VertexPNX(o2, n, Cvec2f(1.0,0.0));
        VertexPNX upperRight = VertexPNX(o3, n, Cvec2f(1.0,1.0));
        VertexPNX upperLeft = VertexPNX(o4, n, Cvec2f(0.0,1.0));

        vtx.push_back(bottomLeft);
        vtx.push_back(bottomRight);
        vtx.push_back(upperRight);
        vtx.push_back(upperLeft);
        
        geoScreen_.reset(new SimpleIndexedGeometryPNX(&vtx[0], &idx[0], vtx.size(), idx.size()));
    }
    
    
    void initCube() {
      int ibLen, vbLen;
      getCubeVbIbLen(vbLen, ibLen);

      // Temporary storage for cube Geometry
      vector<VertexPNTBX> vtx(vbLen);
      vector<unsigned short> idx(ibLen);

      makeCube(1, vtx.begin(), idx.begin());
      geoPrimitiveCube_.reset(new SimpleIndexedGeometryPNTBX(&vtx[0], &idx[0], vbLen, ibLen));
      
    }
    
    void initParticleSystem(int numParticles, float size){
        vector<VertexPNCRX> vtx;
        vector<unsigned int> idx;
        vtx.reserve(4 * numParticles);
        idx.reserve(6 * numParticles);
        float x, y, z;
        vector<VertexPNCRX> quadVertices;
        for (auto i = 0; i < numParticles; i++){
            idx.push_back(i * 4 + 0);
            idx.push_back(i * 4 + 2);
            idx.push_back(i * 4 + 3);
            idx.push_back(i * 4 + 0);
            idx.push_back(i * 4 + 1);
            idx.push_back(i * 4 + 2);
            //generate random particle pos
            x = RandomFloat(-10.0, 10.0);
            y = RandomFloat(-10.0, 10.0);
            z = RandomFloat(-10.0, 10.0);
            quadVertices = getParticleQuadVertices(Cvec3f(x,y,z), size);
            for (int j = 0; j < 4; j++){
              vtx.push_back(quadVertices[j]);
            }
        }
        geoParticleSystem_.reset(new SimpleIndexedGeometryPNCRX(&vtx[0], &idx[0],
                                                                vtx.size(), idx.size()));
        }

    
    vector<VertexPNCRX> getParticleQuadVertices(const Cvec3f & Pos, const float &size){
        float h = 0.5 * size;
        Cvec3f o1 = Cvec3f(-h, -h, 0.0);
        Cvec3f o2 = Cvec3f(h, -h, 0.0);
        Cvec3f o3 = Cvec3f(h, h, 0.0);
        Cvec3f o4 = Cvec3f(-h, h, 0.0);
        Cvec3f n = Cvec3f(0.0,0.0,1.0);

        VertexPNCRX bottomLeft = VertexPNCRX(Pos + o1, n, Pos, o1, Cvec2f(0.0,0.0));
        VertexPNCRX bottomRight = VertexPNCRX(Pos + o2, n, Pos, o2, Cvec2f(1.0,0.0));
        VertexPNCRX upperRight = VertexPNCRX(Pos + o3, n, Pos, o3, Cvec2f(1.0,1.0));
        VertexPNCRX upperLeft = VertexPNCRX(Pos + o4, n, Pos, o4, Cvec2f(0.0,1.0));

        return vector<VertexPNCRX> {bottomLeft, bottomRight, upperRight, upperLeft};
    }
    
};
#endif /* assetlibrary_h */
