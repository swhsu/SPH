/*   DataExtractor.h
 *
 *   Extract data from simulations
 *
 *   Shu-Wei Hsu     Texas A&M University
 */

#ifndef _DATA_EXTRACTOR_H_
#define _DATA_EXTRACTOR_H_

#include "SPHSystem.h"

class DataExtractor
{
public:
	DataExtractor(SPHSystem* sph);
	~DataExtractor(void);
	void extract2D(void);
	void extractGridVel2D(int res1, int res2);

	// [TODO]  extract3D()

	void createFolder(char* folder);

	int numSnapShot;
	char folder[128];
	SPHSystem* sphSystem;
};


#endif