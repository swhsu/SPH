/*  KDTree.h
 *
 *  KDTree for finding neighbors
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#ifndef __KDTREE_H__
#define __KDTREE_H__

#include "PQueue.h"
#include "cyPoint.h"
#include "Particle.h"

using namespace cy;

#define MAX_DIM  20				//max space dimension
#define KD_MAX_POINTS 100000	//max KD tree points number  
#define SD 3					//current space dimension
#define RMAX 512
#define POINTS_NUM 10000
#define TEST_NUM 10000
#define L 0
#define R 1

//#define BUTTONUP

#define ELEM_SWAP(a,b) { register KDNode* t=(a);(a)=(b);(b)=t; }

/*
inline float distance2(Point3f& x1, Point3f& x2, int dim)
{
	float s = 0;
	for(int k=0; k<dim; k++) 
		s += (x1[k]-x2[k])*(x1[k]-x2[k]);
	return s;
}


inline bool equal(Point3f& x1, Point3f& x2, int dim)
{
	for(int k=0; k < dim; k++)
		if(x1[k] != x2[k])
			return false;
	return true ;
}
*/

//KDTreeNode template class implementation
class KDNode
{
public:
	Point3f x;
	int axis ;
	unsigned int id;
	bool checked;
	KDNode* Parent;
	KDNode* Left;
	KDNode* Right;

	KDNode();
	KDNode(Particle x0);
	KDNode(Point3f& x0, int axis0);
	KDNode*	insert(KDNode* x, int LorR);
	KDNode*	findParent(Particle* x0);
};




class KDTree
{
public:

	KDTree(int numParticle=0);
	void build(KDNode** data, int numData, int dim, KDNode* parent, int LorR);
	void store(vector<Particle*> pv);
	void uncheck();
	KDNode*	findNearest(Particle* x);
	KDNode*	findNearestBrute(Particle* x) ;
	KDNode* quickSelect(KDNode** arr, int n, int dim);
	inline void	checkSubtree(KDNode* node, Particle* x);
	inline void	setBoundingCube(KDNode* node, Particle* x);
	inline KDNode*	searchParent(KDNode* parent, Particle* x);


	void locateNearest(Particle* x, KDNode* start, float r);

	void reset() {
		Root = NULL;
		nearestNeighbor = NULL;
		PQ.reset();
		for(int i=0; i<numList; i++) {
			List[i]->Parent = NULL;
			List[i]->Left = NULL;
			List[i]->Right = NULL;
		}

		// NOTE:
		// numList is not able to change since KDTree can not add more node dynamically right now...
		// List[] will be updated by calling store() again.
		// CheckedNodes & numCheckedNodes has been updated in the end of findNearest().
		// minX, maxX, and boundary stuff will be reinitial in next query.
	}

	KDNode* Root ;
	float   minDist ;
	KDNode* nearestNeighbor ;
	PQueue  PQ;

	KDNode* List[KD_MAX_POINTS] ;
	int	    numList ;

	KDNode*	CheckedNodes[KD_MAX_POINTS] ;
	int     numCheckedNodes ;

	Point3f minX, maxX; 
	bool    maxBound[SD], minBound[SD];
	int	    numBound ;

};

#endif