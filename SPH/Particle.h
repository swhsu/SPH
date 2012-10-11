/*  Particle.h
 *
 *  Particles in SPH
 *
 *  Shu-Wei Hsu    Texas A&M University
 */

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include "cyPoint.h"
#include "PQueue.h"

class Particle : public cyPoint3f
{
public:
	Particle(void);
	Particle(cyPoint3f p, int id);
	~Particle(void);

	int id;
	float m;					// mass
	float d;					// density
	float vol;					// volume = mass / density
	float p;					// pressure
	float c;					// color field
	float lapc;					// laplacian of color field
	cyPoint3f v;				// velocity
	cyPoint3f a;				// acceleration
	cyPoint3f n;				// inward surface normal
	PQueue pq;					// neighbors

	// PCISPH
	float dNext;				// next density
	float dErr;					// predicted density error dErr = dNext - dInit (which is 1000 kg/m^3 for water)
	cyPoint3f posNext;			// next position
	cyPoint3f vNext;			// next velocity
	cyPoint3f pf;
	cyPoint3f vf;
	cyPoint3f gf;

	// debug
	cyPoint3f tf;
	float iso;
	cyPoint3f f_no_gravity;
};


#endif
