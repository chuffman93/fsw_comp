#include "util/propagator/AllStarOrbitProp.h"

/*
 * Function: propagatePositionVelocity
 * Purpose: Provides estimate of vehicle position and velocity by propagating
 *  ephemeris or directly using GPS data.
 *
 * Inputs:
 *  gpsPos   = position given by GPS unit, ECI frame (J2000 epoch) [km]
 *  gpsVel   = velocity given by GPS unit, ECI frame (J2000 epoch) [km/s]
 *  TOW      = current GPS TOW provided by the GPS unit[s]
 *  last_TOW = previous GPS TOW provided by the GPS unit [s]
 *  sysTime  = current system time. Time since last Mean Anomaly, given either from last GPS measurement (last_TOW + estimated time using ADCS' internal clock) or last new ephemeris data  [s]
 *  new_eph  = flag for new ephemeris received (1 = new data; 0 = no new data)
 *  new_NORAD_eph = new ephemeris data (COE: a,e,i,Omega,omega)
 *
 * Outputs:
 *  last_TOW = if there is a new GPS solution, last_TOW if modified to the current TOW [s]
 *  sysTime  = current system time, modified to = 0 if we have a new GPS measurement [s]
 *  satPos   = current satellite position, ECI frame (J2000 epoch) [km]
 *  satVel   = current satellite velocity, ECI frame (J2000 epoch) [km]
 */
void propagatePositionVelocity(classicElements oe, float sysTime, float satPos[3], float satVel[3]) {
	int i;
	float M0;			//[rad], initial mean anomaly
	float n;			//[rad/s], initial mean motion
	float r[4];		//[km], most up-to-date position
	float v[4];		//[km/s], most up-to-date velocity

	if (oe.a > REQ_EARTH) {
		/* propagate the last stored epoch */
		M0 = E2M(f2E(oe.anom, oe.e), oe.e);
		n = sqrt(MU_EARTH / oe.a / oe.a / oe.a);
		oe.anom = E2f(M2E(M0 + n * sysTime, oe.e), oe.e);
		elem2rv(MU_EARTH, &oe, r, v);

		/* Convert back to 0 index */
		for (i = 0; i < 3; i++) {
			satPos[i] = r[i + 1];
			satVel[i] = v[i + 1];
		}
	}
}
