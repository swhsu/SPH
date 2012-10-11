/*  KDTree.cpp
 *
 *  KDTree for finding neighbors
 *
 *  Shu-Wei Hsu    Texas A&M University
 */

#include "KDTree.h"
#include "Global.h"

//-------------------------------------------------------------------------------
// KDNode class member function

KDNode::KDNode() { checked = false; Left = Right = Parent = NULL ;}

KDNode::KDNode(Particle x0)
{
	x[0] = x0[0];
	x[1] = x0[1];
	x[2] = x0[2];
	id = x0.id;
	checked = false;
}

KDNode::KDNode(Point3f& x0, int axis0)
{
	axis = axis0;
	for(int k=0; k<SD; k++)
		x[k] = x0[k];

	Left = Right = Parent = NULL ;
	checked = false ;
	id = 0;
}


KDNode*	KDNode::findParent(Particle* x0)
{
	KDNode* parent ;
	KDNode* next = this ;
	int split ;
	while(next) {
		split = next->axis  ;
		parent = next ;
		if((*x0)[split] > next->x[split]) 
			next = next->Right ;
		else 
			next = next->Left ;
	}
	return parent ;
}


KDNode*	KDNode::insert(KDNode* p, int side)
{
	//KDNode* parent = findParent((p->x);
	KDNode* parent = this;
	//if(equal(p->x, parent->x, SD))
	if(p->x == parent->x)
		return NULL ;

	KDNode* newNode = p;
	newNode->Parent = parent ;
	newNode->axis = (parent->axis+1)%SD;

	if(side == L)
		parent->Left = newNode;
	else
		parent->Right = newNode;

	/*
	if(p->x[parent->axis] >= parent->x[parent->axis]) {
	parent->Right = newNode ;
	newNode->orientation = 1 ;
	}
	else {
	parent->Left = newNode ;
	newNode->orientation = 0 ;
	}
	*/

	return newNode ;
}


//-------------------------------------------------------------------------------
// KDTree class member function

KDTree::KDTree(int numParticle) :PQ()
{
	Root = NULL ;
	numList = numParticle;
	for(int i=0; i<numParticle; i++)
		List[i] = new KDNode();
}

void KDTree::store(vector<Particle*> pv)
{
	for(unsigned int i=0; i<pv.size(); i++) {
		List[i]->x.x = pv[i]->x;
		List[i]->x.y = pv[i]->y;
		List[i]->x.z = pv[i]->z;
		List[i]->id  = pv[i]->id;
	}
}

void KDTree::build(KDNode** data, int numData, int dim, KDNode* parent, int LorR)
{
	if(numData<1)
		return;
	if(numData==1) {
		parent->insert(data[0], LorR);
		return;
	}

	KDNode* midNode = quickSelect(data, numData, dim);
	midNode->axis = dim;
	//add(midNode);
	if(!Root)
		Root = midNode;
	else
		parent->insert(midNode, LorR);

	int left = (numData-1)/2;
	int right = numData - left - 1;
	build(data, left, (dim+1)%SD, midNode, L);
	build(&(data[left+1]), right, (dim+1)%SD, midNode, R);
}


KDNode* KDTree::quickSelect(KDNode** arr, int n, int dim) 
{
	int low, high ;
	int median;
	int middle, ll, hh;

	low = 0 ; high = n-1 ; median = (low + high) / 2;
	for (;;) {
		if (high <= low) /* One element only */
			return arr[median];

		if (high == low + 1) {  /* Two elements only */
			if (arr[low]->x[dim] > arr[high]->x[dim])
				ELEM_SWAP(arr[low], arr[high]) ;
			return arr[median];
		}

		/* Find median of low, middle and high items; swap into position low */
		middle = (low + high) / 2;
		if (arr[middle]->x[dim] > arr[high]->x[dim])    ELEM_SWAP(arr[middle], arr[high]) ;
		if (arr[low]->x[dim] > arr[high]->x[dim])       ELEM_SWAP(arr[low], arr[high]) ;
		if (arr[middle]->x[dim] > arr[low]->x[dim])     ELEM_SWAP(arr[middle], arr[low]) ;

		/* Swap low item (now in position middle) into position (low+1) */
		ELEM_SWAP(arr[middle], arr[low+1]) ;

		/* Nibble from each end towards middle, swapping items when stuck */
		ll = low + 1;
		hh = high;
		for (;;) {
			do ll++; while (arr[low]->x[dim] > arr[ll]->x[dim]) ;
			do hh--; while (arr[hh]->x[dim]  > arr[low]->x[dim]) ;

			if (hh < ll)
				break;

			ELEM_SWAP(arr[ll], arr[hh]) ;
		}

		/* Swap middle item (in position low) back into correct position */
		ELEM_SWAP(arr[low], arr[hh]) ;

		/* Re-set active partition */
		if (hh <= median)
			low = ll;
		if (hh >= median)
			high = hh - 1;
	}
}


//sequential nearest neighbour search
KDNode* KDTree::findNearestBrute(Particle* x) 
{
	if(!Root)
		return NULL;

	float d ;
	KDNode* nearest = NULL;
	minDist = 1e10;

	for(int n=0; n<numList; n++) {
		//d =  distance2(List[n]->x, x, SD);
		d = (List[n]->x - (Point3f)(*x)).LengthSquared();
		if(d < H*H) {
			PQ.enQueue(List[n]->id, d);
			if(d<minDist) {
				nearest = List[n];
				minDist = d;
			}
		}
	}

	return nearest ;
}


KDNode* KDTree::findNearest(Particle* x)
{
	if(!Root)
		return NULL ;

#ifdef BUTTONUP
	numCheckedNodes = 0;
	KDNode* parent = Root->findParent(x);
	nearestNeighbor = parent ;
	//minDist = distance2(x, parent->x, SD); ;
	minDist = (parent->x - *x).LengthSquared();

	/*
	//if(equal(x, parent->x, SD)) {
	if(parent->x == (Point3f)(*x)) {
		//PQ.enQueue(parent->id, minDist);
		x->pq.enQueue(parent->id, minDist);
		return nearestNeighbor ;
	}
	*/
	
	searchParent(parent, x);
	uncheck();

#else
	//minDist = distance2(x, Root->x, SD);
	minDist = (Root->x - *x).LengthSquared();
	nearestNeighbor = Root;
	locateNearest(x, Root, H);
#endif

	return nearestNeighbor ;
}




void KDTree::checkSubtree(KDNode* node, Particle* x)
{
	if(!node || node->checked)
		return ;

	CheckedNodes[numCheckedNodes++] = node ;
	node->checked = true ;
	//PQ.enQueue(node->id, (node->x - *x).LengthSquared());
	float dist = (node->x - (Point3f)(*x)).LengthSquared();
	if(dist < H*H)
	x->pq.enQueue(node->id, dist);

	setBoundingCube(node, x);

	int dim = node->axis ;
	float d= node->x[dim] - (*x)[dim];

	if (d*d > minDist) {
		if (node->x[dim] > (*x)[dim])
			checkSubtree(node->Left, x);
		else 
			checkSubtree(node->Right, x);
	}
	// If the distance from the key to the current value is 
	// less than the nearest distance, we still need to look
	// in both directions.
	else {
		checkSubtree(node->Left, x);
		checkSubtree(node->Right, x);
	}
}


void KDTree::setBoundingCube(KDNode* node, Particle* x)
{
	if(!node)
		return ;
	float d=0;
	float dx;
	for(int k=0; k<SD; k++)
	{
		dx = node->x[k]-(*x)[k];
		if(dx > 0)
		{
			dx *= dx ;
			if(!maxBound[k])
			{
				if(dx > maxX[k])
					maxX[k] = dx;
				if(maxX[k]>minDist)
				{
					maxBound[k] =true;
					numBound++ ;
				}
			}
		}
		else 
		{
			dx *= dx ;
			if(!minBound[k])
			{
				if(dx > minX[k])
					//if(dx < minX[k])
					minX[k] = dx;
				if(minX[k]>minDist)
				{
					minBound[k] =true;
					numBound++ ;
				}
			}
		}
		d+=dx;
		if(d>minDist)
			return ;

	}

	if(d<minDist)
	{
		minDist = d;
		nearestNeighbor = node ;
	}
}


KDNode* KDTree::searchParent(KDNode* parent, Particle* x)
{
	for(int k=0; k<SD; k++)
	{
		minX[k] = maxX[k] = 0;
		maxBound[k] = minBound[k] = 0;
	}
	numBound = 0;

	Point3f dx;
	KDNode* search_root = parent ;
	while(parent && numBound != 2*SD)
	{	
		checkSubtree(parent, x);
		search_root = parent ;
		parent = parent->Parent ;
	}

	return search_root ;
}


void KDTree::uncheck()
{
	for(int n=0; n<numCheckedNodes; n++)
		CheckedNodes[n]->checked = false;
}

void KDTree::locateNearest(Particle* x, KDNode* start, float r)
{
	if(start == NULL)
		return;

	float range = r*r;
	int dim = start->axis;
	float dist = (*x)[dim]-start->x[dim];

	// if not leave node
	if(start->Left!=NULL || start->Right!=NULL) {

		// if x in the right of start
		if(dist > 0.0) {
			locateNearest(x, start->Right, r);
			// if the range overlap to the left start, we still have to look at left side
			if(dist*dist < range)
				locateNearest(x, start->Left, r);
		}
		else {
			locateNearest(x, start->Left, r);
			if(dist*dist < range)
				locateNearest(x, start->Right, r);
		}
	}

	// compute squared distance between x and start point.
	float dist2 = (start->x- *x).LengthSquared();

	if(dist2 < range) {
		// good, we found a neighbor. insert it to the PirQueue
		//PQ.enQueue(start->id, dist2);
		x->pq.enQueue(start->id, dist2);

		// save nearest point
		if(dist2 < minDist) {
			minDist = dist2;
			nearestNeighbor = start; 
		}
	}
}
