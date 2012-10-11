/*  LightMat.h
 *
 *  Lighting and material parameters for OpenGL
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#include <GL/freeglut.h>

//--------------------------------------
//  Light parameters
//--------------------------------------

// light 0
static GLfloat light0_position[] = {0, 20, -20, 1.0};
static GLfloat light0_positionInv[] = {10, -1.0, 0, 1.0};
static GLfloat light0_ambient[]  = {0.0, 0.0, 0.0, 1};
static GLfloat light0_diffuse[]  = {1.0, 1.0, 1.0, 1};
static GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1};
// light 1
static GLfloat light1_position[] = {10, 30, 15, 1.0};
static GLfloat light1_ambient[]  = {0.0, 0.0, 0.0, 1};
static GLfloat light1_diffuse[]  = {1.0, 1.0, 1.0, 1};
static GLfloat light1_specular[] = {1.0, 1.0, 1.0, 1};

// global light ambient
static GLfloat lmodel_ambient[]={0.2f, 0.2f, 0.2f, 1.0f};
static GLfloat local_view[]={0.5};

//--------------------------------------
//  Material parameters
//--------------------------------------
static GLfloat mat_black[] = {0, 0, 0, 1};
static GLfloat mat_white[] = {1, 1, 1, 1};
static GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1};
static GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1};
static GLfloat mat_high_shininess[] = {128.0};
static GLfloat mat_low_shininess[] = {16.0};
static GLfloat mat_no_shininess[] = {0.0};

static int mat_color[12][3] = {
	{255, 145,  75},
	{  0, 224,  63},
	{ 63, 112, 229},
	{251,  87, 129},
	{115, 16,   111},
	{251,   0, 255},
	{159,  251, 191},
	{251, 208,  63},
	{123, 176,  47},
	{251,   0,   0},
	{  255, 96,  31},
	{  0,   0, 251}
}; 

