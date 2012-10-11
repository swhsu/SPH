/*  SPHSystem.h
 *
 *  SPH Fluid Simulator
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#ifndef _SPH_SYSTEM_H_
#define _SPH_SYSTEM_H_

#include <vector>
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include "Particle.h"
#include "KDTree.h"
#include "Kernel.h"
#include "Boundary.h"
#include "Global.h"

using namespace Eigen;
using namespace std;

class SPHSystem
{
public:
	SPHSystem(int numParticle = 0);
	~SPHSystem(void);
	void render();

	// SPH
	void initScene();
	void reset();
	void computeNeighbor();
	void computeDensityPressure();
	void computeForce();
	void boundaryEffect();
	void integration();
	void sphLoop();


	// surface
	void volume();
	void dumpVolume();
	void dumpPoints(int numFrame);

	// matrix io
	void exportMatrix(char* filename, MatrixXf* _M);
	void importMatrix(char* filename, MatrixXf* _M);

	int numParticle;
	int displayMode;			// 0: particle  1:surface tension force
	vector<Particle*> g;		// grid
	vector<Particle*> p;		// particles
	KDTree* kdtree;
	Kernel* k;
	Boundary* b;

	int numFrame;

	int simDim;

};

#endif
