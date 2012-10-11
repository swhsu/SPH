/*  Kernel.h
 *
 *  SPH kernels
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <ostream>
#include <cmath>
#include "Global.h"

using namespace std;

class Kernel
{
public:
	float h;   // radius
	float h2;  // radius^2
	float h3;
	float h4;
	float h5;
	float h6;
	float h9;  // radius^9

	Kernel(void) { h = (float)H; h2 = h*h; h3 = h*h*h; h4 = h*h3; h5= h*h4; h6=h*h5; h9=h5*h4; }
	Kernel(float _h) { h = _h; h2 = h*h; h3 = h*h2; h4 = h*h3; h5= h*h4; h6=h*h5; h9=h5*h4; }
	~Kernel(void);


	// for all w1, rember to multiply "d vector"!!
	float w (float d) { if(d<=h) return (315.0f/(64*PI*h9))*pow((h2-d*d),3.0f); else return 0.0f; }
	float w1(float d) { return (-945.0f/(32.0f*PI*h9))*pow((h2-d*d),2.0f); }
	float w2(float d) { if(d<=h) return (-945.0f/(32.0f*PI*h9))*(h2-d*d)*(3.0f*h2-7.0f*d*d); else return 0.0f; }

	// use spikey kernel as pressure kernel
	float pw (float d) { if(d<=h) return (15.0f/(PI*h6))*pow((h-d), 3.0f); else return 0.0f; }
	float pw1(float d) { return (-45.0f/(PI*h6*d))*pow((h-d), 2.0f); }
	float pw2(float d) { if(d<=h) return (-90.0f/(PI*h6*d))*(h-d)*(h-2.0f*d); else return 0.0f; }

	// viscosity kernel
	float vw (float d) { if(d<=h) return (15.0f/(2.0f*PI*h3))*(-d*d*d/(2.0f*h3) + d*d/h2 + h/(2.0f*d) -1.0f); else return 0; }
	float vw1(float d) { return (15.0f/(2.0f*PI*h3))*((-3.0f*d/(2.0f*h3)) + (2.0f/h2) - (h/(2.0f*d*d*d))); }
	float vw2(float d) { return (45.0f/(PI*h6))*(h-d); }

	// surface kernel
	float k(float d) { if(d > 1.0) cerr << "wrong..."; return max(0.0f, pow((1.0f-d*d),3.0f)); }
};

#endif