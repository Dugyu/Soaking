//
//  main.cpp
//  Soaking
//
//  Created by Guangyu Du on 4/25/20.
//  Copyright © 2020 Dugyu. All rights reserved.
//
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

#include "headers/common.h"
#include "headers/glsupport.h"
#include "headers/soak.h"


//---------------------//
//  GLOBAL VARIABLES   //
//---------------------//

std::shared_ptr <Soak> g_soak;
int g_timeSinceStart = 0;

// ----------------- //
//  INITIALIZATION   //
// ----------------- //

static void initApp(){
    g_soak.reset(new Soak());
}
static void initWindow(){
    g_soak->initWindow();
}
static void initGLState() {
    g_soak->initGLState();
}
static void initGraphics(){
    g_soak->initGeometry();
    g_soak->initMaterials();
    g_soak->initScene();
    g_soak->initFrameBuffers();
}

static void updateFrustFovY(){
    g_soak->updateFrustFovY();
}

static void drawStuff(){
    g_soak->drawStuff();
}

static void updateClickInput(const int &button, const int &state, const int &x, const int &y){
    g_soak->updateClickInput(button, state, x, y);
}

// ----------------- //
//      CALLBACKS    //
// ----------------- //

static void display() {


    drawStuff();

    // show the back buffer (where we rendered stuff)
    glutSwapBuffers();
    checkGlErrors();
}

static void reshape(const int w, const int h) {
    g_soak->reshapeWindow(w, h);
    //cerr << "Size of window is now " << w << "x" << h << endl;
    //g_arcballScreenRadius = max(1.0,0.25 * min(g_windowWidth, g_windowHeight));
    updateFrustFovY();
    glutPostRedisplay();
}

static void mouse(const int button, const int state, const int x, const int y) {
    updateClickInput(button, state, x, y);
//    if(g_pickingModeOn && g_mouseLClickButton && !g_mouseRClickButton){
//        pick();
//        if (g_currentPickedRbtNode == g_groundNode || g_currentPickedRbtNode == NULL){
//            g_currentPickedRbtNode = shared_ptr<SgRbtNode>();   // set to NULL
//            g_picked = false;
//            cout<<"No parts picked, picking mode off..."<< endl;
//        }else{
//            g_picked = true;
//            cout<<"You just picked a node, picking mode off..."<< endl;
//        }
//        g_pickingModeOn = false;
//    }
    glutPostRedisplay();
}


static void motion(const int x, const int y) {
//    if (g_activeObject == SKY && g_activeEye != SKY) return;
//    // we do not edit the sky when viewed from the objects
    bool succeed = g_soak->onMotion(x,y);
    if (succeed == false) return;
    glutPostRedisplay();                    // we always redraw if we changed the scene
}


static void particleMovementCallback(int dontCare) {
   
//    for (int i = 0; i< g_numStepsPerFrame; ++i){
//        calcBunnyPhysics();
//    }
//    g_shellNeedsUpdate = true;
//    // schedule this to get called again
    g_timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    g_soak->updateUniforms(g_timeSinceStart);
    glutTimerFunc(1000 / 60, particleMovementCallback, 0);
    glutPostRedisplay(); // signal redisplaying
}


static void initSimulation()
{
    particleMovementCallback(0);
}

// ----------------- //
//      GLUT STATE   //
// ----------------- //

static void initGlutState(int argc, char *argv[]) {
    glutInit(&argc, argv); // initialize Glut based on cmd-line args
#ifdef __APPLE__
    glutInitDisplayMode(
        GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE |
        GLUT_DEPTH); // core profile flag is required for GL 3.2 on Mac
#else
    glutInitDisplayMode(
        GLUT_RGBA | GLUT_DOUBLE |
        GLUT_DEPTH); //  RGBA pixel channels and double buffering
#endif
    initWindow();
    
    
    glutIgnoreKeyRepeat(true); // avoids repeated keyboard calls when holding space to emulate middle mouse
    glutDisplayFunc(display); // display rendering callback
    glutReshapeFunc(reshape); // window reshape callback
    glutMotionFunc(motion);   // mouse movement callback
    glutMouseFunc(mouse);     // mouse click callback
//    glutKeyboardFunc(keyboard);
//    glutKeyboardUpFunc(keyboardUp);
//    glutSpecialFunc(specialKeyboard);
}


// --------------------- //
//      MAIN FUNCTION    //
// --------------------- //

int main(int argc, char *argv[]) {
    try {
        initApp();
        initGlutState(argc, argv);
        initGLState();
        checkGlewVersion();
        initGraphics();
//      initAnimation();
        initSimulation();
        glutMainLoop();
        return 0;
    } catch (const std::runtime_error &e) {
        std::cout << "Exception caught: " << e.what() <<std::endl;
        return -1;
    }
}

