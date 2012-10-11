// cyCodeBase by Cem Yuksel
// [www.cemyuksel.com]
//-------------------------------------------------------------------------------

#ifndef _TIMER_LIST_H_INCLUDED_
#define _TIMER_LIST_H_INCLUDED_

//-------------------------------------------------------------------------------

#include <cstdio>
#include <cfloat>
#include "cyTimer.h"

//-------------------------------------------------------------------------------

// ID of each timer and the number of timers
enum TimerType
{
	NEIGHBOR=0,
	DENSITY,
	FORCE,
	INTEGRATION,
	ONESTEP,

	TIMER_TYPE_COUNT	// this keeps the number of timers (DO NOT REMOVE)
};

// The name string of each timer
static char TimerTypeNames[][255] = {
	"Neighbor",
	"D and P",
	"Force",
	"Update",
	"One Step",
};

//-------------------------------------------------------------------------------

class TimerList
{
public:
	// Clears the data of all timers
	void ClearAll() { Clear( 0, TIMER_TYPE_COUNT ); }

	// Clears the data of the timers between from and to
	void Clear( int from, int to ) { for ( int i=from; i<to; i++ ) timers[i].Clear(); }

	// This is how you access each timer
	cyTimerStats& GetTimer( int type ) { return timers[ type ]; }
	cyTimerStats& operator [] ( int type ) { return GetTimer( type ); }

	// Prints the results of all timers
	void PrintResults( int from=0, int to=TIMER_TYPE_COUNT, FILE *fp=stdout ) {
		for ( int i=from; i<to; i++ ) PrintResult( i, fp );
	}

	// Prints the results of one timer
	void PrintResult( int type, FILE *fp=stdout ) {
		double a = timers[ type ].GetAverage();
		if ( _finite(a) ) {
			fprintf(fp, "%s :  avrg=%8.5f\tmin=%8.5f\tmax=%8.5f\n", TimerTypeNames[ type ], a, timers[type].GetMin(), timers[type].GetMax() );
		} else {
			fprintf(fp, "%s :  avrg=   -----\tmin=   -----\tmax=   -----\n", TimerTypeNames[ type ], a, timers[type].GetMin(), timers[type].GetMax() );
		}
	}
	
protected:
	cyTimerStats timers[ TIMER_TYPE_COUNT ];
};

//-------------------------------------------------------------------------------

extern TimerList TIMER;

//-------------------------------------------------------------------------------

#endif