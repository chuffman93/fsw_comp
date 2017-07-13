
#ifndef _FOR_ALLSTAR_ORBIT_PROP_
#define _FOR_ALLSTAR_ORBIT_PROP_

#define MIN_ORBITAL_VELOCITY 5

#include "stdlib.h"

#include "astroConstants.h"
#include "OrbitalMotionAllStar.h"
#include "vector3DAllStar.h"

#include "math.h"

void propagatePositionVelocity(classicElements oe, float sysTime, float satPos[3], float satVel[3]);

#endif
