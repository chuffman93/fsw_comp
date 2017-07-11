/*
*  astroConstants.h
*  OrbitalMotion
*
*  Created by Hanspeter Schaub on 6/19/05.
*
*  Defines common astronomical constants using in the
*          Keplerian 2-body problem.
*
*/

#ifndef _ASTRO_CONSTANTS_H_
#define _ASTRO_CONSTANTS_H_

/* common conversions */
#ifndef M_PI
#define M_PI    3.141592653589793
#endif
#ifndef M_E
#define M_E     2.718281828459045
#endif
#ifndef D2R
#define D2R     M_PI/180.
#endif
#ifndef R2D
#define R2D     180./M_PI
#endif
#ifndef NAN
#define NAN     sqrt((float)-1)
#endif
#ifndef RPM
#define RPM     (2.*M_PI)/60.
#endif

//#define OMEGA_EARTH     0.00007292115 /* Earth's planetary rotation rate, rad/sec */

/* Zonal gravitational harmonics Ji for the Earth */
#define J2_EARTH        1082.63e-6
#define J3_EARTH       -2.52e-6
#define J4_EARTH       -1.61e-6
#define J5_EARTH       -0.15e-6
#define J6_EARTH        0.57e-6

/* Gravitational Constants mu = G*m, where m is the planet of the attracting planet.  All units are km^3/s^2.
* Values are obtained from SPICE
*/
#define MU_EARTH        398600.436

/* planet information for major solar system bodies. Units are in km.
* data taken from SPICE
*/

/* Earth */
#define REQ_EARTH       6378.1366
#define RP_EARTH        6356.7519

#endif
