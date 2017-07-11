/*
*  orbitalMotion.h
*  OrbitalMotion
*
*  Created by Hanspeter Schaub on 6/19/05.
*
*  This package provides various orbital
*  mechanics subroutines using in astrodynamics calculations.
*
*/

#ifndef _ORBITAL_MOTION_H_
#define _ORBITAL_MOTION_H_

#define N_DEBYE_PARAMETERS 37

typedef struct classicElem {
	float a;			// Semi-major axis							[km]
	float e;			// Eccentricity
	float i;			// Inclination								[rad]
	float Omega;		// Right ascension of the ascending node	[rad]
	float omega;		// Argument of perigee						[rad]
	float anom;			// True anomaly								[rad]
} classicElements;

float  E2f(float E, float e);
float  E2M(float E, float e);
float  f2E(float f, float e);
float  f2H(float f, float e);
float  H2f(float H, float e);
float  H2N(float H, float e);
float  M2E(float M, float e);
float  N2H(float N, float e);
void    elem2rv(float mu, classicElements *elements, float *rVec, float *vVec);
void    rv2elem(float mu, float *rVec, float *vVec, classicElements *elements);

float  atmosphericDensity(float alt);
float  debyeLength(float alt);
void    atmosphericDrag(float Cd, float A, float m, float *rvec, float *vvec, float *advec);
void    jPerturb(float *rvec, int num, float *ajtot);
void    solarRad(float A, float m, float *sunvec, float *arvec);

#endif
