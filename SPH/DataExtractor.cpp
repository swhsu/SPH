/*   DataExtractor.cpp
 *
 *   Extract data from simulations
 *
 *   Shu-Wei Hsu     Texas A&M University
 */


#include <iostream>
#include <fstream>
#include <windows.h>
#include "DataExtractor.h"

using namespace std;

DataExtractor::DataExtractor(SPHSystem* sph)
{
	this->sphSystem = sph;
	numSnapShot = 0;
}


DataExtractor::~DataExtractor(void)
{
}


void DataExtractor::createFolder(char* folderName)
{
	strcpy_s(folder, folderName);
	if(CreateDirectory(folderName,NULL) == 0) 
	{
		cerr << "create folder error..." << endl;
		exit(0);
	}
}



void DataExtractor::extract2D()
{
	char tmp1[256];
	char tmp2[256];
	char tmp3[256];
	char tmp4[256];
	
	sprintf_s(tmp1, "./%s/%04d.dat\0", folder, numSnapShot);				// positions
	sprintf_s(tmp2, "./%s/force_%04d.dat\0", folder, numSnapShot);			// force
	sprintf_s(tmp3, "./%s/vel_%04d.dat\0", folder, numSnapShot);			// vel
	sprintf_s(tmp4, "./%s/neihbor_%04d.dat\0", folder, numSnapShot);		// neighbor info
	ofstream file1(tmp1, ios::binary);
	ofstream file2(tmp2, ios::binary);
	ofstream file3(tmp3, ios::binary);
	ofstream file4(tmp4, ios::binary);

	if( !file1.is_open() || !file2.is_open() || !file3.is_open() || !file4.is_open()) 
	{
		cerr << "extract2D data error..." << endl;
		exit(0);
	}
	
	file4.write((char*)(&sphSystem->numParticle), sizeof(int));
	for(int i=0; i<sphSystem->numParticle; i++) 
	{
		file1.write((char*)&(sphSystem->p[i]->x), sizeof(float));		// output pos x
		file1.write((char*)&(sphSystem->p[i]->y), sizeof(float));		// output pos y

		file2.write((char*)&(sphSystem->p[i]->a.x), sizeof(float));		// output acc x
		file2.write((char*)&(sphSystem->p[i]->a.y), sizeof(float));		// output acc y

		file3.write((char*)&(sphSystem->p[i]->v.x), sizeof(float));		// output vel x
		file3.write((char*)&(sphSystem->p[i]->v.y), sizeof(float));		// output vel y

		int nsize = sphSystem->p[i]->pq.getSize();
		file4.write((char*)(&nsize), sizeof(int));
		file4.write((char*)(&(sphSystem->p[i]->pq.queue[1])), nsize*sizeof(int));
	}

	file1.close();
	file2.close();
	file3.close();
	file4.close();
}



void DataExtractor::extractGridVel2D(int res1, int res2)
{
	int numGrids = res1*res2;		// assume it's 1024
	
	float bound = 6.4f;
	float gap = bound/res1;
	float v[1024*2];
	int count[1024];

	memset(v, 0, sizeof(float)*1024*2);
	memset(count, 0, sizeof(float)*1024);


	// 1. map force to grids

	for(int i=0; i<sphSystem->numParticle; i++) 
	{
		float x = sphSystem->p[i]->x;
		float y = sphSystem->p[i]->y;

		if(x<0) x = 0;
		if(y<0) y = 0;
		if(x>6.3) x = 6.3f;
		if(y>6.3) y = 6.3f;
		x = floor(x/gap);
		y = floor(y/gap);

		int idx = (int)(y*32+x);

		v[2*idx]   += sphSystem->p[i]->v.x;  count[idx]++;
		v[2*idx+1] += sphSystem->p[i]->v.y;  
	}


	// average
	for(int i=0; i<2*numGrids; i++) 
	{
		if(count[i/2] != 0)
			v[i] /= count[i/2];
	}

	// output average v
	char tmp[256];
	sprintf_s(tmp, "./%s/avgGridVel_%04d.dat\0", folder, numSnapShot);
	
	ofstream file(tmp, ios::binary);
	if( !file.is_open() || !file.is_open()) 
	{
		cerr << "export snap error..." << endl;
		exit(0);
	}
	file.write((char*)v, sizeof(float)*2*numGrids);
	file.close();

	// output num of particles in each grid
	char tmp2[256];
	sprintf_s(tmp2, "./%s/numParticleInGrid_%04d.dat\0", folder, numSnapShot);
	ofstream file2(tmp2, ios::binary);
	if( !file2.is_open() || !file2.is_open()) 
	{
		cerr << "export snap error (part2)..." << endl;
		exit(0);
	}

	file2.write((char*)count, sizeof(int)*numGrids);
	file2.close();
}