/*   Boundary.h
 *
 *   Cubic boundary for SPH simulations
 *
 *   Shu-Wei Hsu     Texas A&M University
 */

#ifndef _BOUNDARY_H_
#define _BOUNDARY_H_

#include "Particle.h"
#include "cyPoint.h"

using namespace cy;


class Boundary
{
public:
	Boundary(void);
	~Boundary(void);
	bool isInside(Point3f* p);
	void correctV(Particle* p);
	void render();

	float w;
	float h;
	float d;

	Point3f n[6];
};

#endif