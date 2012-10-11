// sheepy CG land by Shu-Wei Hsu
//-------------------------------------------------------------------------------
///
/// \file		sheepyCamera.h 
/// \author		Shu-Wei Hsu
/// \version	1.0
/// \date		Jan 27, 2010
///
/// \brief Camera module for OpenGL
///
///
/// sheepyCamera is a camera class for OpenGL.
/// 
/// Call setPos() first, and then minpulate camera by related functions. 
///  
///  
///  
/// Some standard settings for the 35mm camera. 
/// ratio: 1.5
/// focal Length: 13    18    24    28    35    50    70    85    105  135   200   300
/// vertical ang: 85.4  67.4  53.1  46.4  37.8  27.0  19.5  16.1   13  10.2  6.87  4.58
///
///
///
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
#ifndef _SHEEPY_CAMERA_H_
#define _SHEEPY_CAMERA_H_

#include "cyPoint.h"
#include "Global.h"
#include <GL/freeglut.h>


using namespace cy;
//-------------------------------------------------------------------------------


//-------------------------------------------------------------------------------
/// \class sheepyCamera
class sheepyCamera
{
public :
	float ratio;
	float focalLen;
	float verticalAngle;
	float dist;
	float rx; 
	float ry;
	Point3f pos;
	Point3f lookat;
	Point3f dir;
	Point3f up;
	

	/// \name Constructors
	sheepyCamera(){
		ratio=1.5f; 
		focalLen = 0.001f*50.f;
		verticalAngle = 27.f;
		setProj();
	}

	sheepyCamera(float _r, float _f, float _v):ratio(_r), focalLen(_f*0.001f), verticalAngle(_v){
		setProj();
	}

	~sheepyCamera(){}

	/// \name Camera Setting
	void setProj() {
		double vAngle,halfFilmH;
		double _left,_right,_top,_bottom,_near=focalLen,_far=500;

		vAngle = 0.5*PI*verticalAngle/180;
		halfFilmH     = _near * tan(vAngle);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		_left  = - ratio * halfFilmH;
		_right =   ratio * halfFilmH;
		_top    =   halfFilmH;
		_bottom = - halfFilmH;
		glFrustum(_left,_right,_bottom,_top,_near,_far);		
	}

	void setRatio(float _ratio) {ratio = _ratio;}

	void setView() {
		glMatrixMode(GL_MODELVIEW);
		glDrawBuffer(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		gluLookAt(   pos[0],    pos[1],    pos[2],
							lookat[0], lookat[1], lookat[2],
							    up[0],     up[1],     up[2]);  
	}
	
	/// \name Camera manipulation

	void setPos(float _rx, float _ry, float _dist, Point3f _lookat){
		lookat=_lookat;
		rx=_rx;
		ry=_ry;
		dist=_dist;
		calcPos();
		//calcFrame();
		up=Point3f(0,1.0,0);
	}

	void rotate(float dry, float drx) {
		ry += dry; 
		rx += drx;
		if (rx>(PI*0.5f)-0.01f) rx=PI*0.5f-0.01f;
		if (rx<-(PI*0.5f)+0.01f) rx=-PI*0.5f+0.01f;
		calcPos();
		//calcFrame();
	}

	void moveDist(float ddist) {
		dist+=ddist;
		if (dist<1) dist=1;
		calcPos();
	}

	void moveDrive(float dx, float dy) {
		Point3f screen_right, screen_up;
		screen_right=dir.Cross(up);
		screen_up=screen_right.Cross(dir);

		screen_right.Normalize();
		screen_up.Normalize();

		lookat+=-dx*screen_right+-dy*screen_up;
		calcPos();
	}

	/// \name get film position
	void getFilmCorner(Point3f& RD, Point3f& LD, Point3f& RT, Point3f& LT) {
		Point3f screen_right, screen_up;
		screen_right=dir.Cross(up);
		screen_up=screen_right.Cross(dir);
		screen_right.Normalize();
		screen_up.Normalize();		
		float vAngle = 0.5f*PI*verticalAngle/180;
		float halfFilmH  = focalLen * tan(vAngle);
		float halfFilmW  = ratio * halfFilmH;

		RT = pos + dir*focalLen + screen_right*halfFilmW + screen_up*halfFilmH;
		RD = pos + dir*focalLen + screen_right*halfFilmW - screen_up*halfFilmH;
		LT = pos + dir*focalLen - screen_right*halfFilmW + screen_up*halfFilmH;
		LD = pos + dir*focalLen - screen_right*halfFilmW - screen_up*halfFilmH;
	}

private :
	void calcPos(){
		float cX=cos(rx)*dist;						//spherical coordinate
		pos[0]=lookat[0]+cX*sin(ry);
		pos[1]=lookat[1]+sin(rx)*dist;
		pos[2]=lookat[2]+cX*-cos(ry);
		dir=lookat - pos;
		dir.Normalize();
	}

	void calcFrame() {
		Point3f screen_right;
		dir.Normalize();
		screen_right=dir.Cross(up);
		up=screen_right.Cross(dir);	
		up.Normalize();
	}
};
//-------------------------------------------------------------------------------


namespace sheepy {
	typedef sheepyCamera Camera;
}

#endif