/*   Boundary.cpp
 *
 *   Cubic boundary for SPH simulations
 *
 *   Shu-Wei Hsu     Texas A&M University
 */

#include "Boundary.h"
#include "Global.h"
#include <GL/freeglut.h>

Boundary::Boundary(void)
{

	w = W_BOUNDARY;
	h = H_BOUNDARY;
	d = D_BOUNDARY;


	n[0].Set(1,0,0);
	n[1].Set(-1,0,0);
	n[2].Set(0,1,0);
	n[3].Set(0,-1,0);
	n[4].Set(0,0,1);
	n[5].Set(0,0,-1);
}

Boundary::~Boundary(void)
{
}

bool Boundary::isInside(Point3f* p)
{
	if(p->x <0 || p->x>w || p->y<0 || p->y>h || p->z<0 || p->z>d)
		return false;
	return true;
}

void Boundary::render()
{
	glBegin(GL_QUADS);
	// button
	glVertex3f(0,0,0);
	glVertex3f(0,0,d);
	glVertex3f(w,0,d);
	glVertex3f(w,0,0);
	// left
	glVertex3f(0,0,0);
	glVertex3f(0,h,0);
	glVertex3f(0,h,d);
	glVertex3f(0,0,d);
	// right
	glVertex3f(w,0,0);
	glVertex3f(w,h,0);
	glVertex3f(w,h,d);
	glVertex3f(w,0,d);
	// back
	glVertex3f(0,0,0);
	glVertex3f(w,0,0);
	glVertex3f(w,h,0);
	glVertex3f(0,h,0);
	// top
	glVertex3f(0,0,d);
	glVertex3f(w,0,d);
	glVertex3f(w,h,d);
	glVertex3f(0,h,d);
	glEnd();
}

void Boundary::correctV(Particle* p)
{
	float rCoe = RES;

	// left 
	if(p->x<0) {
		p->x = -(p->x);
		p->v.x *=-rCoe;
	}

	// right
	else if(p->x>w) {
		p->x = 2*w - p->x;
		p->v.x *=-rCoe;
	}

	// button 
	if(p->y<0) {
		p->y = -(p->y);
		p->v.y *=-rCoe;
	}
	// top 3 
	else if(p->y>h) {
		p->y = 2*h - p->y;
		p->v.y *=-rCoe;
	}

	// back 
	if(p->z<0) {
		p->z = -p->z;
		p->v.z *= -rCoe;
	}
	// back 
	else if(p->z>d) {
		p->z = 2*d - p->z;
		p->v.z *= -rCoe;
	}
}

