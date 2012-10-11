/*  PQueue.h
 *
 *  Priority Queue
 *
 *  Shu-Wei Hsu    Texas A&M University
 */


#ifndef _P_QUEUE_
#define _P_QUEUE_

#include <vector>

#define LEFT		-1
#define RIGHT		 1
#define BOTH		 0
#define MAX_QUEUE_SIZE 300

using namespace std;

// Top-Min Priority Queue

class PQueue
{
private:
	int qsize;
	void bubbleUp(int startCell);
	void bubbleDown(int startCell);
	bool isParentBigger(int childCell);
	bool isLChildSmaller(int parentCell);
	bool isRChildSmaller(int parentCell);
	int  compareChild(int parentCell);

public:
	PQueue(void);
	~PQueue(void);
	int getSize();
	void enQueue(int e, float weight);
	int deQueue();
	int top();
	void reset();
	//int remove(int cell);

	int queue[MAX_QUEUE_SIZE];
	float weight[MAX_QUEUE_SIZE];
};


#endif
