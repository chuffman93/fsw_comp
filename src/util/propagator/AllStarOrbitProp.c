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
void propagatePositionVelocity(float gpsPos[3], float gpsVel[3], float TOW,
		float *last_TOW, float *sysTime, int *new_eph,
		classicElements *new_NORAD_eph, float satPos[3], float satVel[3]) {
	classicElements oe;
	int i;
	float M0;			//[rad], initial mean anomaly
	float n;			//[rad/s], initial mean motion
	float r[4];		//[km], most up-to-date position
	float v[4];		//[km/s], most up-to-date velocity
	float rGps[4];	//[km], GPS position
	float vGps[4];	//[km/s], GPS velocity

	/* The GPS unit outputs the accuracy of the measurement. CDH rejects the GPS solution if the accuracy is too low.
	 ADCS only receives good GPS solutions from CDH and it stores the TOW. If TOW is the same as the last one used, ADCS did not received
	 a new GPS solution. Otherwise, ADCS uses the new GPS solution, updates the time to the new TOW and restarts sysTime to zero.*/

	/* Check if we got a new GPS solution */
	if (*last_TOW != TOW) {
		/* There is no propagation. We use the new GPS position and TOW. */
		/* Although CDH gives good GPS solutions base on the accuracy provided by the GPS unit,
		 check to make sure that they are good measurements */

		// Convert to 1 index: to work w/ other functions
		for (i = 0; i < 3; i++) {
			rGps[i + 1] = gpsPos[i];
			vGps[i + 1] = gpsVel[i];
		}

		// General, common sense checks
		if (norm_as(rGps) < REQ_EARTH) {
			return;
		}

		if (norm_as(vGps) < MIN_ORBITAL_VELOCITY) {
			return;
		}

		// Replace all the parameters with the new GPS solution
		*last_TOW = TOW;
		*sysTime = 0;
		new_NORAD_eph->anom = 0;
		for (i = 0; i < 3; i++) {
			satPos[i] = gpsPos[i];
			satVel[i] = gpsVel[i];
		}
	}

	else { // No new GPS solution, so propagate position and velocity using the last ephemeris data and sysTime

		if (*new_eph) { // There is new ephemeris data. So propagate from here (given M0) until we have a new GPS solution
			oe.a = new_NORAD_eph->a;
			oe.e = new_NORAD_eph->e;
			oe.i = new_NORAD_eph->i;
			oe.Omega = new_NORAD_eph->Omega;
			oe.omega = new_NORAD_eph->omega;
			/* Include the true anomaly */

			*sysTime = 0;// Reset the sysTime, so we keep track of the time from this Mean anomaly.
			*new_eph = 0;// It remains zero until CDH uploads new ephemeris. Until then, use last GPS solution
		} else { // The ephemeris is older than the last GPS solution. So use the last GPS pos and veloc
			// Convert to 1 index: to work w/ other functions
			for (i = 0; i < 3; i++) {
				rGps[i + 1] = gpsPos[i];
				vGps[i + 1] = gpsVel[i];
			}

			r[1] = rGps[1];
			r[2] = rGps[2];
			r[3] = rGps[3];
			v[1] = vGps[1];
			v[2] = vGps[2];
			v[3] = vGps[3];
			rv2elem(MU_EARTH, r, v, &oe);// Get the orbital elements using the last position and velocity
		}

		if (oe.a > REQ_EARTH) {
			/* propagate the last stored epoch */
			M0 = E2M(f2E(oe.anom, oe.e), oe.e);
			n = sqrt(MU_EARTH / oe.a / oe.a / oe.a);
			oe.anom = E2f(M2E(M0 + n * *sysTime, oe.e), oe.e);
			elem2rv(MU_EARTH, &oe, r, v);

			/* Convert back to 0 index */
			for (i = 0; i < 3; i++) {
				satPos[i] = r[i + 1];
				satVel[i] = v[i + 1];
			}
		}
	}
}
