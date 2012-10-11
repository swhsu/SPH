/*  PQueue.cpp
 *
 *  Priority Queue
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#include "PQueue.h"

PQueue::PQueue(void)
{
	// array based priority queue implementation
	// make the queue start from index 1
	qsize = 0;
	queue[0] = 0;
	weight[0] = 0;
}

PQueue::~PQueue(void)
{
}

void PQueue::reset()
{
	qsize = 0;
}

int PQueue::getSize()
{
	return qsize;
}

int PQueue::top()
{
	if(qsize == 0)
		return NULL;
	else
		return queue[1];
}


void PQueue::enQueue(int e, float w)
{
	if(qsize>=MAX_QUEUE_SIZE) {
		printf("PQueue Full...");
		exit(-1);
	}

	qsize++;
	queue[qsize] = e;
	weight[qsize] = w;
	
	// mark
	//e->pqIndex = getSize();

	bubbleUp(qsize);
}

int PQueue::deQueue()
{
	if(qsize == 0)
		return NULL;
	else {
		int min = queue[1];

		if(qsize>1) {
			int lastElem = queue[qsize];
			float lastW = weight[qsize];
			queue[1] = lastElem;
			weight[1] = lastW;
			
			qsize--;

			// mark
			//lastElem->pqIndex = 1;
			
			// heapify
			bubbleDown(1);

		} else {
			//queue.erase(queue.begin()+1);
			qsize = 0;
		}

		// mark
		//min->pqIndex = -1;

		return min;
	}
}


void PQueue::bubbleUp(int cell)
{
	while(isParentBigger(cell)) {
		// prepare for swap
		int parent = queue[cell/2];
		int child  = queue[cell];
		float parentW = weight[cell/2];
		float childW  = weight[cell];

		// swap
		queue[cell/2] = child;
		queue[cell]   = parent;
		weight[cell/2] = childW;
		weight[cell]   = parentW;

		// mark
		//child->pqIndex = cell/2;
		//parent->pqIndex = cell;

		// go up one level
		cell /= 2;
	}
}


void PQueue::bubbleDown(int cell)
{
	while(isLChildSmaller(cell) || isRChildSmaller(cell)) {
		
		int child = compareChild(cell);

		if(child==LEFT) {
			// prepare swap
			int parent = queue[cell];
			int lChild = queue[2*cell];
			float parentW = weight[cell];
			float lChildW = weight[2*cell];

			// swap
			queue[cell]   = lChild;
			queue[2*cell] = parent;
			weight[cell]   = lChildW;
			weight[2*cell] = parentW;			

			// mark
			//lChild->pqIndex = cell;
			//parent->pqIndex = 2*cell;

			// go down one level
			cell = 2*cell;
		} else if (child == RIGHT) {
			// prepare swap
			int parent = queue[cell];
			int rChild = queue[2*cell+1];
			float parentW = weight[cell];
			float rChildW = weight[2*cell+1];
			
			// swap
			queue[cell]   = rChild;
			queue[2*cell+1] = parent;
			weight[cell]   = rChildW;
			weight[2*cell+1] = parentW;

			// mark
			//rChild->pqIndex = cell;
			//parent->pqIndex = 2*cell+1;

			// go down one level
			cell = 2*cell+1;
		}
	}
}

inline bool PQueue::isParentBigger(int childCell)
{
	if( childCell == 1 )		// top, no parent
		return false;
	else
		return weight[childCell/2] > weight[childCell];
	
}

inline bool PQueue::isLChildSmaller(int parentCell)
{
	if( (parentCell*2) > qsize)
		return false;	// no child
	else
		return weight[2*parentCell] < weight[parentCell];
}

inline bool PQueue::isRChildSmaller(int parentCell)
{
	if( (parentCell*2+1) > qsize)
		return false;	// no child
	else
		return weight[2*parentCell+1] < weight[parentCell];	
}

// check which child is smaller
int PQueue::compareChild(int parentCell)
{
	bool lChildSmaller = isLChildSmaller(parentCell);
	bool rChildSmaller = isRChildSmaller(parentCell);

	if(lChildSmaller || rChildSmaller) {
		// chick which one is smaller
		if(lChildSmaller && rChildSmaller) {
			int lChild = 2*parentCell;
			int rChild = 2*parentCell+1;
			float left  = weight[lChild];
			float right = weight[rChild];
			
			//if(left->err <= right->err)
			if(left <= right)
				return LEFT;
			else
				return RIGHT;
		} else if (lChildSmaller)
			return LEFT;
		else
			return RIGHT;
	}
	return BOTH;
}

/*
int PQueue::remove(int cell)
{
	if(getSize() == 0)
		return NULL;
	else {
		T* target = queue[cell];
		if(getSize()>cell) {
			Edge* lastElem = queue[queue.getSize()-1];
			queue.pop_back(); // delete last elem
			queue[cell] = lastElem;

			// mark
			lastElem->pqIndex = cell;

			// heapify
			bubbleDown(cell);

		} else {
			queue.erase(queue.begin()+cell);
		}

		// mark
		target->pqIndex = -1;

		return target;
	}
}
*/