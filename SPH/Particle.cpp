/*  Particle.cpp
 *
 *  Particles in SPH
 *
 *  Shu-Wei Hsu    Texas A&M University
 */

#include "Particle.h"
#include "Global.h"

Particle::Particle(void)
{
	this->x = 0;
	this->y = 0;
	this->z = 0;

	this->id = 0;

	this->m  = M;
	this->c  = 1;
	this->d  = RHO;
	this->dNext = RHO;
	this->p  = 0;
	this->v.Zero(); 
	this->a.Zero();
}

Particle::Particle(cyPoint3f p, int id)
{
	this->x = p.x;
	this->y = p.y;
	this->z = p.z;

	this->id = id;

	this->m  = M;
	this->c  = 1;
	this->d  = RHO;
	this->dNext = RHO;
	this->p  = 0;
	this->v.Zero(); 
	this->a.Zero();
}

Particle::~Particle(void)
{
}
