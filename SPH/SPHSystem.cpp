/*  SPHSystem.cpp
 *
 *  SPH Fluid Simulator
 *
 *  Shu-Wei Hsu    Texas A&M University
 */

#include "Timerlist.h"
#include "SPHSystem.h"
#include "Debug.h"
#include <omp.h>
#include <iostream>
#include <fstream>
#include <GL/freeglut.h>


SPHSystem::SPHSystem(int _numParticle)
{
	numParticle = _numParticle;
	kdtree = new KDTree(numParticle);
	k = new Kernel();
	b = new Boundary();

	displayMode = 0;
	numFrame = 0;
}


SPHSystem::~SPHSystem(void)
{
	for(unsigned int i=0; i<p.size(); ++i)
		delete p[i];
}



void SPHSystem::exportMatrix(char* filename, MatrixXf* _M)
{
	// output matrix row by row.

	int row = (int)_M->rows();
	int col = (int)_M->cols();
	float tmp = 0;
	

	ofstream output(filename, ios::binary);
	
	output.write((char*)&row, sizeof(int));
	output.write((char*)&col, sizeof(int));

	for(int i=0; i<row; i++) {
		for(int j=0; j<col; j++) {
			tmp = (*_M)(i,j);
			output.write((char*)&tmp, sizeof(float));			
		}
	}
	output.close();
}

void SPHSystem::importMatrix(char* filename, MatrixXf* _M)
{
	// input matrix row by row
	int row;
	int col;
	float* data;


	ifstream input(filename, ios::binary);

	input.read((char*)&row, sizeof(int));
	input.read((char*)&col, sizeof(int));
	data = new float[row*col];
	input.read((char*)data, sizeof(float)*row*col);

	_M->resize(row, col);

	for(int i=0; i<row; i++) {
		for(int j=0; j<col; j++) {
			(*_M)(i,j) = data[i*col+j];
		}
	}
	
	delete [] data;
	input.close();
}


void SPHSystem::sphLoop()
{
	TIMER[NEIGHBOR].Start();
	computeNeighbor();
	TIMER[NEIGHBOR].Stop();

	TIMER[DENSITY].Start();
	computeDensityPressure();
	TIMER[DENSITY].Stop();
	
	TIMER[FORCE].Start();
	computeForce();
	TIMER[FORCE].Stop();

	TIMER[INTEGRATION].Start();
	integration();
	TIMER[INTEGRATION].Stop();

	numFrame++;
}


void SPHSystem::initScene()
{

	if(SIM_DIM==2)
	{
		// [Scenario] 2D (dam break from left) 
		Particle* _p;
		float px, py, pz;
		for(int i=0; i<numParticle; ++i) {
			py = 1.0f + i/50 + 0.5f;
			pz = 0.5f;
			px = i%50 + 0.5f;
			_p = new Particle(Point3f(0.1f*px,0.1f*py,0.1f*pz), i);
			p.push_back(_p);
		}

		// [Scenario] 2D (dam break from right) 
		/*
		Particle* _p;
		float px, py, pz;
		for(int i=0; i<numParticle; ++i) {
			py = 1.0f + i/32 + 0.5f;
			pz = 0.5f;
			px = 48 + i%32 + 0.5f;
			_p = new Particle(Point3f(0.1f*px,0.1f*py,0.1f*pz), i);
			p.push_back(_p);
		}
		*/
	}

	else
	{
		// [Scenario] 3D (dam break from one corner) 
		Particle* _p;
		float px, py, pz;
		for(int i=0; i<numParticle; ++i) {
			py = (float)(i/400);
			pz = (float)((i/20)%20);
			px = (float)(i%20);
			_p = new Particle(Point3f(0.1f*px,0.1f*py,0.1f*pz), i);
			p.push_back(_p);
		}

		// grid
		Particle* _g;
		float gx, gy, gz;
		int w=64, d=1, h=64;
		for(int i=0; i<w; ++i) {
			gx = i+0.5f;
			for(int j=0; j<h; ++j) {
				gy = j+0.5f;
				for(int k=0; k<d; ++k) {
					gz = k+0.5f;
					_g = new Particle(Point3f(0.1f*gx, 0.1f*gy, 0.1f*gz), 0);
					_g->iso = 99999;
					g.push_back(_g);
				}
			}
		}
	}
}

void SPHSystem::reset()
{
	if(SIM_DIM==2)
	{
		float px, py, pz;
		for(int i=0; i<numParticle; ++i) {
			py =  5.0f + i/50 + 0.5f;
			pz = 0.5f;
			px = i%50 + 0.5f;
			p[i]->Set(0.1f*px, 0.1f*py, 0.1f*pz);
			p[i]->p = 0;
			p[i]->c = 1;
			p[i]->d = RHO;
			p[i]->a.Zero();
			p[i]->v.Zero();
		}
	}
	else
	{
		float px, py, pz;
		for(int i=0; i<numParticle; ++i) {
			py = (float)(i/20);
			pz = (float)((i/20)%20);
			px = (float)(i%20);
			p[i]->Set(0.01f*px, 0.01f*py, 0.01f*pz);
			p[i]->p = 0;
			p[i]->c = 1;
			p[i]->d = p[i]->dNext = p[i]->dErr = RHO;
			p[i]->vNext.Zero();
			p[i]->posNext.Zero();
			p[i]->a.Zero();
			p[i]->v.Zero();
			p[i]->pf.Zero();
			p[i]->vf.Zero();
			p[i]->gf.Zero();
			p[i]->tf.Zero();
		}
	}
}

void SPHSystem::computeNeighbor()
{
	kdtree->reset();
	kdtree->store(p);
	kdtree->build(kdtree->List, numParticle, 0, NULL, NULL);

	#pragma omp parallel for
	for(int i=0; i<p.size(); ++i) {
		p[i]->pq.reset();
		kdtree->findNearest(p[i]);
	}
}

void SPHSystem::computeDensityPressure()
{
	int i, j, s, numNei;
	float lapc, c, d;
	float w;
	float vj;       // volumn
	float lenij;    // length (i,j)
	Point3f n;
	Point3f vecij;  // vector (i,j)
	
	for(i=0; i<p.size(); ++i) {
		p[i]->vol = p[i]->m/p[i]->d;
	}

	#pragma omp parallel for private(s, j, numNei, lapc, c, d, w, vj, lenij, n, vecij )
	for(i=0; i<p.size(); ++i) {
		lapc = 0;
		c = 0;
		d = 0;
		n.Zero();
		numNei = p[i]->pq.getSize();
		for(s=1; s<=numNei; ++s) {
			j = p[i]->pq.queue[s];
			vj = p[j]->vol;
			vecij = *p[i]-*p[j];
			lenij = vecij.Length();
			w = k->w(lenij);
			
			c += p[j]->c*p[j]->m*(vj)*w;
			n += p[j]->m*(vj)*k->w1(lenij)*(vecij);
			lapc += p[j]->m*(vj)*k->w2(lenij);
			
			d += p[j]->m*w;
		}
		p[i]->lapc = lapc;										// laplacian of c
		p[i]->c = c;											// color field
		p[i]->n = n;											// inward surface normal
		
		p[i]->d = d;											// density
		p[i]->p = (K*RHO/7.0f)*(pow((d/RHO),7.0f)-1);           // pressure (Tait Equation, http://graphics.stanford.edu/~wicke/eg09-tutorial/coursenotes.pdf)
		if(p[i]->p < 0)											// correct pressure if p<0
			p[i]->p *= Z;										// reduce it to a tiny number (~=0)
			
	}

}


void SPHSystem::computeForce()
{
	int i, j, s, numNei;
	float dij;
	float term1, term2;
	Point3f vij;
	Point3f pf, vf, tf, gf, cf;

	//#pragma omp parallel for private( j, nId, numNei, d, term1, term2, dv, pf, vf, tf, gf )
	for(i=0; i<p.size(); ++i) {
		pf.Set(0,0,0);
		vf.Set(0,0,0);
		tf.Set(0,0,0);
		numNei = p[i]->pq.getSize();

		
		for(s=1; s<=numNei; ++s) {
			j = p[i]->pq.queue[s];
			if(i==j) continue;
			term1 = p[i]->p/(p[i]->d*p[i]->d);
			term2 = p[j]->p/(p[j]->d*p[j]->d);
			vij = *p[i]-*p[j];
			dij = vij.Length();
			// pressure force
			pf += (term1+term2)*p[j]->m*k->pw1(dij)*vij;
			// viscosity force
			vf += p[j]->m*k->vw2(dij)*(p[j]->v-p[i]->v);
			// surface tension			
		}

		
		pf = -p[i]->d*pf;
		vf = (X/p[i]->d)*vf;
		gf = p[i]->d*Point3f(0.0f, -GR, 0.0f);
		
		if(p[i]->n.Length() < ( SIM_DIM==2 ? 0.6f : 3.0f) )		// for 2d < 0.6 ,   3d < 3.0
			tf.Zero();
		else
			tf = -p[i]->lapc*p[i]->n.GetNormalized();
		
		p[i]->tf = tf;
		p[i]->pf = pf;

		p[i]->vf = vf;
		p[i]->a = (pf+vf+gf+tf)/p[i]->d;
	}
}

void SPHSystem::integration()
{
	Point3f pos;
	for(unsigned int i=0; i<p.size(); ++i) {

		// integrate velocity
		p[i]->v = p[i]->v + DT*p[i]->a;

		// integration positions
		pos.x = p[i]->x + DT*p[i]->v.x;
		pos.y = p[i]->y + DT*p[i]->v.y;
		pos.z = p[i]->z + DT*p[i]->v.z;


		// constraint
		if(!b->isInside(&pos)) {
			b->correctV(p[i]);
			pos.x = p[i]->x + DT*p[i]->v.x;
			pos.y = p[i]->y + DT*p[i]->v.y;
			pos.z = p[i]->z + DT*p[i]->v.z;
		}
		
		// update position
		p[i]->x = pos.x;
		p[i]->y = pos.y;
		p[i]->z = pos.z;
	}
}



void SPHSystem::render()
{
	if(displayMode == 0) {
		glBegin(GL_POINTS);
		for(unsigned int i=0; i<p.size(); ++i) {

			glColor3f(p[i]->dNext/1000, p[i]->dNext/1000, p[i]->dNext/1000);
			glVertex3fv(&((*p[i])[0]));
			//glVertex3f(p[i]->x+ 0.5*p[i]->v.x, p[i]->y+ 0.5*p[i]->v.y, p[i]->z+ 0.5*p[i]->v.z);
		}
		glEnd();
		glFlush();


		// draw velocity/pressure of particle0's neighbors

		float scale = 1e-2f;
		int numSample = 10;
		vector<int> selectedId;
		for(int i=0; i<numSample; i++)
		{
			selectedId.push_back(i*191+19);
		}

		for(int i=0; i<numSample; i++)
		{
			int nsize = p[selectedId[i]]->pq.getSize();
			for(int j=1; j<=nsize; j++)
			{
				int neigId = p[selectedId[i]]->pq.queue[j];
				glPushMatrix();
				glTranslatef(p[neigId]->x, p[neigId]->y, p[neigId]->z);
				glBegin(GL_LINES);
				glColor3f(0,1,0);
				glVertex3f(0, 0, 0);
				glVertex3f(p[neigId]->v.x, p[neigId]->v.y, p[neigId]->v.z);
				glColor3f(1,0,0);
				glVertex3f(0, 0, 0);
				glVertex3f(scale*p[neigId]->a.x, scale*p[neigId]->a.y, scale*p[neigId]->a.z);
				glEnd();
				glPopMatrix();
			}
		}
	}



	// surface normal
	else if(displayMode == 1) {
		glBegin(GL_LINES);
		for(unsigned int i=0; i<p.size(); ++i) {
			//glColor3f(log(p[i]->c*1.3), log(p[i]->c*1.3), 1);
			glVertex3fv(&((*p[i])[0]));
			p[i]->n.Normalize();
			glVertex3f(p[i]->x+(p[i]->tf[0])*0.001f, p[i]->y+(p[i]->tf[1])*0.001f, p[i]->z+(p[i]->tf[2])*0.001f);
		}
		glEnd();
		glFlush();
	}

	// grid position
	else if(displayMode == 2) {
		glColor3f(1,0,0);
		glBegin(GL_POINTS);
		for(int i=0; i<g.size(); ++i) {
			glVertex3f(g[i]->x, g[i]->y, g[i]->z);
		}
		glEnd();
		glFlush();
	}

	// volume
	else if(displayMode == 3) {
		float max=-999, min=999;
		for(int i=0; i<g.size(); ++i) {
			if( g[i]->iso > isoValue) {
				glColor3f(10*g[i]->iso, 0, 0);
				glPushMatrix();
				glTranslatef(g[i]->x, g[i]->y, g[i]->z);
				glutWireCube(0.1);
				glPopMatrix();
				if(g[i]->iso > max)
					max = g[i]->iso;
				if(g[i]->iso < min)
					min = g[i]->iso;
			}
		}
		//cerr << "max:" << max << "  min:" << min << "  isoThres:" << isoValue << endl;
	}

	//// tracing neighbor
	//glColor3f(1,0,0);
	//glBegin(GL_POINTS);
	//for(int j=1; j<=p[pick]->pq.getSize(); ++j) {
	//	int nId = p[pick]->pq.queue[j];
	//	glVertex3f(p[nId]->x, p[nId]->y, p[nId]->z);
	//}
	//glEnd();
	//glFlush();
}

void SPHSystem::boundaryEffect()
{
	#pragma omp parallel for
	for(int i=0; i<p.size(); ++i)
		b->correctV(p[i]);
}

void SPHSystem::volume()
{
	// build tree
	kdtree->reset();
	kdtree->store(p);
	kdtree->build(kdtree->List, numParticle, 0, NULL, NULL);

	for(int i=0; i<g.size(); ++i) {
		g[i]->pq.reset();
		kdtree->findNearest(g[i]);
	}

	// compute iso value on grid points
	int nId;
	int numNei=0;
	float d;
	float w, k1, k2;
	float r_;
	Point3f x_;

	printf("compute iso value...\n");
	for(int i=0; i<g.size(); ++i) {
		printf("\b\b\b\b\b\b");
		printf("%2.2f", 100.0f*(float)i/g.size());
		numNei = g[i]->pq.getSize();
		k2 = 0;
		r_ = 0;
		x_.Zero();

		// sigma j K
		for(int j=1; j<=numNei; ++j) {
			nId = g[i]->pq.queue[j];
			d = (*p[nId]-*g[i]).Length();
			k2 += k->k(d/H);
		}
		for(int j=1; j<=numNei; ++j) {
			nId = g[i]->pq.queue[j];
			d = (*p[nId]-*g[i]).Length();
			k1 = k->k(d/H);
			w = k1/k2;
			r_ += w*0.1f;
			x_ += w*(*p[nId]);
		}
		g[i]->iso = r_ - (*g[i]-x_).Length();
	}

}

void SPHSystem::dumpVolume()
{
	ofstream file("volume.txt", ios::binary);
	
	// res
	file.write((char*)&(b->w), sizeof(int));
	file.write((char*)&(b->h), sizeof(int));
	file.write((char*)&(b->d), sizeof(int));

	for(int i=0; i<g.size(); i++) {
		file.write((char*)&(g[i]->iso), sizeof(float));
	}

	file.close();
}

void SPHSystem::dumpPoints(int numFrame)
{
	char buf[1024];
	sprintf_s(buf, "points_frame_%04d.dat", numFrame);
	ofstream file(buf, ios::binary);

	int numPoints = (int)(p.size());
	file.write((char*)&(numPoints), sizeof(int));
	for(int i=0; i<p.size(); i++) {
		file.write((char*)&(p[i]->x), sizeof(float));
		file.write((char*)&(p[i]->y), sizeof(float));
		file.write((char*)&(p[i]->z), sizeof(float));
	}

	file.close();
}