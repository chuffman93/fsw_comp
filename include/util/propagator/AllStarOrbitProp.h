
#ifndef _FOR_ALLSTAR_ORBIT_PROP_
#define _FOR_ALLSTAR_ORBIT_PROP_

#define MIN_ORBITAL_VELOCITY 5

#include "stdlib.h"

#include "astroConstants.h"
#include "OrbitalMotionAllStar.h"
#include "vector3DAllStar.h"
//#include "CDH/BufferPool.h"

#include "math.h"

void propagatePositionVelocity(float gpsPos[3], float gpsVel[3], float TOW, float *last_TOW, float *sysTime, int *new_eph, classicElements *new_NORAD_eph, float satPos[3], float satVel[3]);
int isEqual(float lastVal[4], float currentVal[4]);

#endif
