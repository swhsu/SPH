/*   Global.h
 *
 *   Global simulation settings
 *
 *   Shu-Wei Hsu     Texas A&M University
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//#define ENABLE_3D 

// Constant
//-------------------------------------------------------------
#define PI (3.14159265358979323846f)
#define DEG_TO_RAD 0.0174532925f
#define RAD_TO_DEG 57.2957795f

// SPH parameter
//-------------------------------------------------------------
#define DT 0.001f         // time step
#define M 1.0f            // mass of one particle
#define RHO	1000.0f       // rest density  (kg/m^3)   water = 1000
#define Z 0.000001f       // pressure scaling for negative pressure
#define RES 0.200f
#define GR 9.8f

#ifdef ENABLE_3D
#define SIM_DIM 3
#define H 0.23f           // kernel radius	2D:0.2  3D:0.23
#define X 45.0f           // viscosity	2D:35  3D:45
#define K 120.0f          // stiffness	2D:80 3D:120
#define W_BOUNDARY 3.2f
#define H_BOUNDARY 3.2f
#define D_BOUNDARY 3.2f
#define NUM_PARTICLE 10000

#else
#define SIM_DIM 2
#define H 0.16f           // kernel radius	2D:0.16  3D:0.23
#define X 25.0f           // viscosity	2D:35  3D:45
#define K 50.0f           // stiffness	2D:80 3D:120
#define W_BOUNDARY 6.4f
#define H_BOUNDARY 6.4f
#define D_BOUNDARY 0.1f
#define NUM_PARTICLE 2000

#endif


extern float isoValue;
extern int pick;

#endif
