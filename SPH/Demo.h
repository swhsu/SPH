
/*  Demo.h
 *
 *  SPH Simulation Demo App
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#ifndef _DEMO_H_
#define _DEMO_H_

#include <iostream>
#include "DataExtractor.h"
#include "SPHSystem.h"
#include "sheepyCamera.h"
#include "LightMat.h"
#include <GL/freeglut.h>

using namespace std;
using namespace cy;
using namespace sheepy;

int winW=1280;
int winH=720;

Camera cam;
int lastx=0;
int lasty=0;
unsigned char Buttons[3] = {0};

// Axes
float ORG[3] = {0,0,0};
float XP[3] = {1,0,0}, XN[3] = {-1,0,0},
YP[3] = {0,1,0}, YN[3] = {0,-1,0},
ZP[3] = {0,0,1}, ZN[3] = {0,0,-1};

// sph
SPHSystem* sph;

// data extractor
DataExtractor* dataExtractor;
bool isExtractData = false;

// sim control
int simStart = 0;

// functions
void glInit();
void lightInit();
void simInit();
void simLoop();

// debug
float isoValue=0;
int pick=0;

// timer
TimerList TIMER;

#endif

