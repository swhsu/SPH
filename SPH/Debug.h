
/*  Debug.h
 *
 *  printing utility for debuging
 *
 *  Shu-Wei Hsu    Texas A&M University
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <cstdio>
#include <cfloat>

enum Var
{
	NUM_ERR_PARTICLE=0,
	PCT_ERR_PARTICLE,
	NUM_ERR_DENSITY,
	PCT_ERR_DENSITY,

	VAR_COUNT	// this keeps the number of variables (DO NOT REMOVE)
};

static char VarNames[][255] = {
	"numErrParticle",
	"pctErrParticle",
	"totalErrDensity",
	"pctErr",
};

class DEBUG
{
public:
	static float variable[VAR_COUNT];
	static void reset() {
		for ( int i=0; i<VAR_COUNT; i++)
			variable[i] = 0;
	}
	static void dump_all( int from=0, int to=VAR_COUNT, FILE *fp=stdout ) {
		for ( int i=from; i<to; i++ ) dump( i, fp );
	}
	static void dump( int type, FILE *fp=stdout ) {
		float a = variable[ type ];
		if ( _finite(a) ) {
			fprintf(fp, "%s :  %8.3f\n", VarNames[ type ], variable[ type ] );
		} else {
			fprintf(fp, "%s :  -----", VarNames[ type ] );
		}
	}
};

float DEBUG::variable[VAR_COUNT];

#endif