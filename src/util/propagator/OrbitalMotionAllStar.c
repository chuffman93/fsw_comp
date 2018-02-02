/*
* orbitalMotion.c
* OrbitalMotion
*
* Created by Hanspeter Schaub on 6/19/05.
* Copyright (c) 2005 Hanspeter Schaub. All rights reserved.
*
*/

#include "util/propagator/OrbitalMotionAllStar.h"

#include <math.h>
#include <stdio.h>

#include "util/propagator/astroConstants.h"
#include "util/propagator/vector3DAllStar.h"

/*
*   f = E2f(Ecc, e)
*
*  Maps eccentric anomaly angles into true anomaly angles.
*  This function requires the orbit to be either circular or
*  non-rectilinar elliptic orbit.
*
*  Input is
*      Ecc - eccentric anomaly    (rad)
*      e   - eccentricity          0 <= e < 1
*
*  Output is
*      f   - true anomaly         (rad)
*/
float E2f(float Ecc, float e)
{
	float f;

	if ((e >= 0) && (e < 1)) {
		f = 2 * atan2(sqrt(1 + e) * sin(Ecc / 2), sqrt(1 - e) * cos(Ecc / 2));
	}
	else {
		f = NAN;
		printf("ERROR: E2f() received e = %g \n", e);
		printf("The value of e should be 0 <= e < 1. \n");
	}

	return f;
}

/*
*  M = E2M(Ecc, e)
*
*  Maps the eccentric anomaly angle into the corresponding
*  mean elliptic anomaly angle.  Both 2D and 1D elliptic
*  orbit are allowed.
*
*  Input is
*      Ecc - eccentric anomaly    (rad)
*      e   - eccentricity          0 <= e <= 1
*
*  Output is
*      M   - mean elliptic anomaly (rad)
*/
float E2M(float Ecc, float e)
{
	float M;

	if ((e >= 0) && (e < 1)) {
		M = Ecc - e * sin(Ecc);
	}
	else {
		M = NAN;
		printf("ERROR: E2M() received e = %g \n", e);
		printf("The value of e should be 0 <= e < 1. \n");
	}

	return M;
}

/*
*  Ecc = f2E(f, e)
*
*  Maps true anomaly angles into eccentric anomaly angles.
*  This function requires the orbit to be either circular or
*  non-rectilinar elliptic orbit.
*
*  Input is
*      f   - true anomaly angle   (rad)
*      e   - eccentricity          0 <= e < 1
*
*  Output is
*      Ecc - eccentric anomaly     (rad)
*/
float f2E(float f, float e)
{
	float Ecc;

	if ((e >= 0) && (e < 1)) {
		Ecc = 2 * atan2(sqrt(1 - e) * sin(f / 2), sqrt(1 + e) * cos(f / 2));
	}
	else {
		Ecc = NAN;
		printf("ERROR: f2E() received e = %g \n", e);
		printf("The value of e should be 0 <= e < 1. \n");
	}

	return Ecc;
}

/*
*  H = f2H(f, e)
*
*  Maps true anomaly angles into hyperbolic anomaly angles.
*  This function requires the orbit to be hyperbolic
*
*  Input is
*      f   - true anomaly angle   (rad)
*      e   - eccentricity          e > 1
*
*  Output is
*      H   - hyperbolic anomaly   (rad)
*/
float f2H(float f, float e)
{
	float H;

	if (e > 1) {
		H = 2 * arc_tanh_as(sqrt((e - 1) / (e + 1)) * tan(f / 2));
	}
	else {
		H = NAN;
		printf("ERROR: f2H() received e = %g \n", e);
		printf("The value of e should be 1 < e. \n");
	}

	return H;
}

/*
*  f = H2f(H, e)
*
*  Maps hyperbolic anomaly angles into true anomaly angles.
*  This function requires the orbit to be hyperbolic
*
*  Input is
*      H   - hyperbolic anomaly   (rad)
*      e   - eccentricity          e > 1
*
*  Output is
*      f   - true anomaly         (rad)
*/
float H2f(float H, float e)
{
	float f;

	if (e > 1) {
		f = 2 * atan(sqrt((e + 1) / (e - 1)) * tanh(H / 2));
	}
	else {
		f = NAN;
		printf("ERROR: H2f() received e = %g \n", e);
		printf("The value of e should be 1 < e \n");
	}

	return f;
}

/*
*  N = H2N(H, e)
*
*  Maps the hyperbolic anomaly angle H into the corresponding
*  mean hyperbolic anomaly angle N.
*
*  Input is
*      H   - hyperbolic anomaly        (rad)
*      e   - eccentricity              e > 1
*
*  Output is
*      N   - mean hyperbolic anomaly   (rad)
*/
float H2N(float H, float e)
{
	float N;

	if (e > 1) {
		N = e * sinh(H) - H;
	}
	else {
		N = NAN;
		printf("ERROR: H2N() received e = %g \n", e);
		printf("The value of e should be 1 < e. \n");
	}

	return N;
}

/*
*  Ecc = M2E(M, e)
*
*  Maps the mean elliptic anomaly angle into the corresponding
*  eccentric anomaly angle.  Both 2D and 1D elliptic
*  orbit are allowed.
*
*  Input is
*      M   - mean elliptic anomaly     (rad)
*      e   - eccentricity              0 <= e <= 1
*
*  Output is
*      Ecc - eccentric anomaly         (rad)
*/
float M2E(float M, float e)
{
	float small = 1e-8;
	float dE = 10 * small;
	float E1 = M;
	int    max = 200;
	int    count = 0;

	if ((e >= 0) && (e < 1)) {
		while (fabs(dE) > small) {
			dE = (E1 - e * sin(E1) - M) / (1 - e * cos(E1));
			E1 -= dE;
			if (++count > max) {
				//printf("iteration error in M2E(%f,%f)\n", M, e);
				dE = 0.;
			}
		}
	}
	else {
		E1 = NAN;
		printf("ERROR: M2E() received e = %g \n", e);
		printf("The value of e should be 0 <= e <= 1. \n");
	}

	return E1;
}

/*
*  H = N2H(N, e)
*
*  Maps the mean hyperbolic anomaly angle N into the corresponding
*  hyperbolic anomaly angle H.
*
*  Input is
*      N   - mean hyperbolic anomaly   (rad)
*      e   - eccentricity              e > 1
*
*  Output is
*      H   - hyperbolic anomaly        (rad)
*/
float N2H(float N, float e)
{
	float small = 1e-13;
	float dH = 10 * small;
	float H1 = N;
	int    max = 200;
	int    count = 0;

	if (e > 1) {
		while (fabs(dH) > small) {
			dH = (e * sinh(H1) - H1 - N) / (e * cosh(H1) - 1);
			H1 -= dH;
			if (++count > max) {
				printf("iteration error in N2H(%f,%f)\n", N, e);
				dH = 0.;
			}
		}
	}
	else {
		H1 = NAN;
		printf("ERROR: N2H() received e = %g \n", e);
		printf("The value of e should be 0 <= e <= 1. \n");
	}

	return H1;
}

/*
*  elem2rv(mu, *element, *rVec, *vVec)
*
*  Translates the orbit elements
*           a   - semi-major axis           (km)
*           e   - eccentricity
*           i   - inclination               (rad)
*           AN  - ascending node            (rad)
*           AP  - argument of periapses     (rad)
*           f   - true anomaly angle        (rad)
*  to the inertial Cartesian position and velocity vectors.
*  The attracting body is specified through the supplied
*  gravitational constant mu (units of km^3/s^2).
*
*  The code can handle the following cases:
*      circular:       e = 0           a > 0
*      elliptical-2D:  0 < e < 1       a > 0
*      elliptical-1D:  e = 1           a > 0        f = Ecc. Anom. here
*      parabolic:      e = 1           rp = -a
*      hyperbolic:     e > 1           a < 0
*
*  Note: to handle the parabolic case and distinguish it form the
*  rectilinear elliptical case, instead of passing along the
*  semi-major axis a in the "a" input slot, the negative radius
*  at periapses is supplied.  Having "a" be negative and e = 1
*  is a then a unique identified for the code for the parabolic
*  case.
*/
void elem2rv(float mu, classicElements *elements, float *rVec, float *vVec)
{
	float e;
	float a;
	float Ecc;
	float f;
	float r;
	float v;
	float i;
	float rp;
	float p;
	float AP;
	float AN;
	float theta;
	float h;
	float ir[3 + 1];

	/* map classical elements structure into local variables */
	a = elements->a;
	e = elements->e;
	i = elements->i;
	AN = elements->Omega;
	AP = elements->omega;
	f = elements->anom;

	/* TODO: Might want to have an error band on this equality */
	if ((e == 1.0) && (a > 0.0)) {   /* rectilinear elliptic orbit case */
		Ecc = f;                    /* f is treated as ecc. anomaly */
		r = a * (1 - e * cos(Ecc)); /* orbit radius  */
		v = sqrt(2 * mu / r - mu / a);
		ir[1] = cos(AN) * cos(AP) - sin(AN) * sin(AP) * cos(i);
		ir[2] = sin(AN) * cos(AP) + cos(AN) * sin(AP) * cos(i);
		ir[3] = sin(AP) * sin(i);
		mult_as(r, ir, rVec);
		if (sin(Ecc) > 0) {
			mult_as(-v, ir, vVec);
		}
		else {
			mult_as(v, ir, vVec);
		}
	}
	else {
		if ((e == 1) && (a < 0)) {   /* parabolic case */
			rp = -a;                /* radius at periapses  */
			p = 2 * rp;             /* semi-latus rectum */
		}
		else {                    /* elliptic and hyperbolic cases */
			p = a * (1 - e * e);    /* semi-latus rectum */
		}

		r = p / (1 + e * cos(f)); /* orbit radius */
		theta = AP + f;           /* true latitude angle */
		h = sqrt(mu * p);     /* orbit ang. momentum mag. */

		rVec[1] = r * (cos(AN) * cos(theta) - sin(AN) * sin(theta) * cos(i));
		rVec[2] = r * (sin(AN) * cos(theta) + cos(AN) * sin(theta) * cos(i));
		rVec[3] = r * (sin(theta) * sin(i));

		vVec[1] = -mu / h * (cos(AN) * (sin(theta) + e * sin(AP)) + sin(AN) * (cos(theta) + e * cos(AP)) * cos(i));
		vVec[2] = -mu / h * (sin(AN) * (sin(theta) + e * sin(AP)) - cos(AN) * (cos(theta) + e * cos(AP)) * cos(i));
		vVec[3] = -mu / h * (-(cos(theta) + e * cos(AP)) * sin(i));
	}
	return;
}

/*
*  rv2elem(mu, *rVec, *vVec, *elements)
*
*  Translates the orbit elements inertial Cartesian position
*  vector rVec and velocity vector vVec into the corresponding
*  classical orbit elements where
*           a   - semi-major axis           (km)
*           e   - eccentricity
*           i   - inclination               (rad)
*           AN  - ascending node            (rad)
*           AP  - argument of periapses     (rad)
*           f   - true anomaly angle        (rad)
*                 if the orbit is rectilinear, then this will be the
*                 eccentric or hyperbolic anomaly
*  The attracting body is specified through the supplied
*  gravitational constant mu (units of km^3/s^2).
*
*  The code can handle the following cases:
*      circular:       e = 0           a > 0
*      elliptical-2D:  0 < e < 1       a > 0
*      elliptical-1D:  e = 1           a > 0
*      parabolic:      e = 1           a = -rp
*      hyperbolic:     e > 1           a < 0
*
*  For the parabolic case the semi-major axis is not defined.
*  In this case -rp (radius at periapses) is returned instead
*  of a.  For the circular case, the AN and AP are ill-defined,
*  along with the associated ie and ip unit direction vectors
*  of the perifocal frame. In this circular orbit case, the
*  unit vector ie is set equal to the normalized inertial
*  position vector ir.
*/
/* TODO: (SAO) Modify this code to return true longitude of periapsis
*  (non-circular, equatorial), argument of latitude (circular, inclined),
*  and true longitude (circular, equatorial) when appropriate instead of
*  simply zeroing out omega and Omega */
void rv2elem(float mu, float *rVec, float *vVec, classicElements *elements)
{
	float r;
	float h;
	float eps;
	float ai;
	float p;
	float rp;
	float Ecc;
	float H;
	float ir[3 + 1];
	float hVec[3 + 1];
	float cVec[3 + 1];
	float dum[3 + 1];
	float ih[3 + 1];
	float ie[3 + 1];
	float ip[3 + 1];
	float dum2[3 + 1];

	/* define a small number */
	eps = 0.000000000001;

	/* compute orbit radius */
	r = norm_as(rVec);
	mult_as(1. / r, rVec, ir);

	/* compute the angular momentum vector */
	cross_as(rVec, vVec, hVec);
	h = norm_as(hVec);

	/* compute the eccentricity vector */
	cross_as(vVec, hVec, cVec);
	mult_as(-mu / r, rVec, dum);
	add_as(cVec, dum, cVec);
	elements->e = norm_as(cVec) / mu;

	/* compute semi-major axis */
	ai = 2. / r - dot_as(vVec, vVec) / mu;
	if (fabs(ai) > eps) {
		/* elliptic or hyperbolic case */
		elements->a = 1 / ai;
	}
	else {
		/* parabolic case */
		p = h * h / mu;
		rp = p / 2;
		elements->a = -rp;   /* a is not defined for parabola, so -rp is returned instead */
		elements->e = 1;
	}

	if (h < eps) {   /* rectilinear motion case */
		equal_as(ir, ie);
		/* ip and ih are arbitrary */
		set3_as(0, 0, 1, dum);
		set3_as(0, 1, 0, dum2);
		cross_as(ie, dum, ih);
		cross_as(ie, dum2, ip);
		if (norm_as(ih) > norm_as(ip)) {
			mult_as(1 / norm_as(ih), ih, ih);
		}
		else {
			mult_as(1 / norm_as(ip), ip, ih);
		}
		cross_as(ih, ie, ip);

	}
	else {
		/* compute perifocal frame unit direction vectors */
		mult_as(1. / h, hVec, ih);
		if (fabs(elements->e) > eps) {
			/* non-circular case */
			mult_as(1. / mu / elements->e, cVec, ie);
		}
		else {
			/* circular orbit case.  Here ie, ip are arbitrary, as long as they
			are perpenticular to the ih vector.  */
			equal_as(ir, ie);
		}
		cross_as(ih, ie, ip);
	}

	/* compute the 3-1-3 orbit plane orientation angles */
	elements->Omega = atan2(ih[1], -ih[2]);
	elements->i = acos(ih[3]);
	elements->omega = atan2(ie[3], ip[3]);

	if (h < eps) {                       /* rectilinear motion case */
		if (ai > 0) {                    /* elliptic case */
			Ecc = acos(1 - r * ai);
			if (dot_as(rVec, vVec) > 0)
				Ecc = 2 * M_PI - Ecc;
			elements->anom = Ecc;       /* for this mode the eccentric anomaly is returned */
		}
		else {                        /* hyperbolic case */
			H = arc_cosh_as(r * ai + 1);
			if (dot_as(rVec, vVec) < 0)
				H = 2 * M_PI - H;
			elements->anom = H;         /* for this mode the hyperbolic anomaly is returned */
		}
	}
	else {
		/* compute true anomaly */
		cross_as(ie, ir, dum);
		elements->anom = atan2(dot_as(dum, ih), dot_as(ie, ir));
	}

	return;
}

/*
*  atmosphericDensity (float alt)
*
*  Purpose:   This program computes the atmospheric density based on altitude
*             supplied by user.  This function uses a curve fit based on
*             atmospheric data from the Standard Atmoshere 1976 Data. This
*             function is valid for altitudes ranging from 100km to 1000km.
*
*             Note: This code can only be applied to spacecraft orbiting
*             the Earth
*
*  Curve fit equation based on data obtained from:
*  U.S. Standard Atmosphere, 1976, U.S. Government Printing Office, Washington, D.C., 1976.
*
*  Input is
*      alt -  This is the altitude supplied by the user in km
*
*  Output is
*      density  - This is the density at the given altitude in kg/m^3
*/
float atmosphericDensity(float alt)
{
	float logdensity;
	float density;
	float val;

	/* Smooth exponential drop-off after 1000 km */
	if (alt > 1000.) {
		logdensity = (-7e-05) * alt - 14.464;
		density = pow(10., logdensity);
		return density;
	}

	/* Calculating the density based on a scaled 6th order polynomial fit to the log of density */
	val = (alt - 526.8000) / 292.8563;
	logdensity = 0.34047 * pow(val, 6) - 0.5889 * pow(val, 5) - 0.5269 * pow(val, 4)
		+ 1.0036 * pow(val, 3) + 0.60713 * pow(val, 2) - 2.3024 * val - 12.575;

	/* Calculating density by raising 10 to the log of density */
	density = pow(10., logdensity);

	return density;
}

/*
*  debyeLength (alt)
*
*  Purpose:   This program computes the debyeLength length for a given
*             altitude and is valid for altitudes ranging
*             from 200 km to GEO (35000km).  However, all values above
*             1000 km are HIGHLY speculative at this point.
*
*  Input is
*      alt -  This is the altitude supplied by the user in km
*
*  Output is
*      debye  - This is the debye length given in m
*/
float debyeLength(float alt)
{
	float debyedist;
	float a;
	float X[N_DEBYE_PARAMETERS] = { 200, 250, 300, 350, 400, 450, 500, 550,
		600, 650, 700, 750, 800, 850, 900, 950, 1000, 1050, 1100, 1150,
		1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1650, 1700,
		1750, 1800, 1850, 1900, 1950, 2000
	};
	float Y[N_DEBYE_PARAMETERS] = { 5.64E-03, 3.92E-03, 3.24E-03, 3.59E-03,
		4.04E-03, 4.28E-03, 4.54E-03, 5.30E-03, 6.55E-03, 7.30E-03, 8.31E-03,
		8.38E-03, 8.45E-03, 9.84E-03, 1.22E-02, 1.37E-02, 1.59E-02, 1.75E-02,
		1.95E-02, 2.09E-02, 2.25E-02, 2.25E-02, 2.25E-02, 2.47E-02, 2.76E-02,
		2.76E-02, 2.76E-02, 2.76E-02, 2.76E-02, 2.76E-02, 2.76E-02, 3.21E-02,
		3.96E-02, 3.96E-02, 3.96E-02, 3.96E-02, 3.96E-02
	};
	int i;

	/* Flat debyeLength length for altitudes above 2000 km */
	if ((alt > 2000) && (alt <= 30000)) {
		alt = 2000;
	}
	else if ((alt > 30000) && (alt <= 35000)) {
		debyedist = 0.1 * alt - 2999.7;
		return debyedist;
	}
	else if ((alt < 200) || (alt > 35000)) {
		printf("ERROR: debyeLength() received alt = %g\n", alt);
		printf("The value of alt should be in the range of [200 35000].\n");
		debyedist = NAN;
		return debyedist;
	}

	/* Interpolation of data */
	for (i = 0; i < N_DEBYE_PARAMETERS - 1; i++) {
		if (X[i + 1] > alt) break;
	}
	a = (alt - X[i]) / (X[i + 1] - X[i]);
	debyedist = Y[i] + a * (Y[i + 1] - Y[i]);

	return debyedist;
}

/*
*  atmosphericDrag (Cd,A,m,rvec,vvec, advec)
*
*  Purpose:   This program computes the atmospheric drag acceleration
*             vector acting on a spacecraft.
*             Note the acceleration vector output is inertial, and is
*             only valid for altitudes up to 1000 km.
*             Afterwards the drag force is zero. Only valid for Earth.
*
*  Input is
*      Cd -  This is the drag coefficient of the spacecraft
*      A  -  This is the cross-sectional area of the spacecraft in m^2
*      m  -  This is the mass of the spacecraft in kg
*      rvec - Inertial position vector of the spacecraft in km  [x;y;z]
*      vvec - Inertial velocity vector of the spacecraft in km/s [vx;vy;vz]
*
*  Output is
*      advec  - The inertial acceleration vector due to atmospheric
*               drag in km/sec^2
*
*/
void atmosphericDrag(float Cd, float A, float m, float *rvec, float *vvec, float *advec)
{
	float r;
	float v;
	float alt;
	float ad;
	float density;

	/* find the altitude and velocity */
	r = norm_as(rvec);
	v = norm_as(vvec);
	alt = r - REQ_EARTH;

	/* Checking if user supplied a orbital position is inside the earth */
	if (alt <= 0.) {
		printf("ERROR: atmosphericDrag() received rvec = [%g %g %g] \n", rvec[1], rvec[2], rvec[3]);
		printf("The value of rvec should produce a positive altitude for the Earth.\n");
		set3_as(NAN, NAN, NAN, advec);
		return;
	}

	/* get the Atmospheric density at the given altitude in kg/m^3 */
	density = atmosphericDensity(alt);

	/* compute the magnitude of the drag acceleration */
	ad = ((-0.5) * density * (Cd * A / m) * (pow(v * 1000., 2))) / 1000.;

	/* computing the vector for drag acceleration */
	mult_as(ad / v, vvec, advec);

	return;
}

/*
*  jPerturb(rvec,num,ajtot)
*
*  Purpose:  Computes the J2_EARTH-J6_EARTH zonal graviational perturbation
*            accelerations.
*
*  Input is
*      rvec - Cartesian Position vector in kilometers [x;y;z].
*      num  - Corresponds to which J components to use,
*             must be an integer between 2 and 6.
*             (note: Additive- 2 corresponds to J2_EARTH while 3 will
*             correspond to J2_EARTH + J3_EARTH)
*
*
*  Output is
*      ajtot  - The total acceleration vector due to the J
*               perturbations in km/sec^2 [accelx;accely;accelz]
*/
void jPerturb(float *rvec, int num, float *ajtot)
{
	float mu, req, x, y, z, r, temp[4], temp2[4];

	/* Constants for Earth */
	mu = MU_EARTH;
	req = REQ_EARTH;

	/* Calculate the J perturbations */
	x = rvec[1];
	y = rvec[2];
	z = rvec[3];
	r = norm_as(rvec);

	/* Error Checking */
	if ((num < 2) || (num > 6)) {
		printf("ERROR: jPerturb() received num = %d \n", num);
		printf("The value of num should be 2 <= num <= 6. \n");
		set3_as(NAN, NAN, NAN, ajtot);
		return;
	}

	/* Calculating the total acceleration based on user input */
	if (num >= 2) {
		set3_as((1 - 5 * pow(z / r, 2))*(x / r), (1 - 5 * pow(z / r, 2))*(y / r), (3 - 5 * pow(z / r, 2))*(z / r), ajtot);
		mult_as(-3. / 2.*J2_EARTH*(mu / pow(r, 2))*pow(req / r, 2), ajtot, ajtot);
	}
	if (num >= 3) {
		set3_as(5 * (7 * pow(z / r, 3) - 3 * (z / r))*(x / r), 5 * (7 * pow(z / r, 3) - 3 * (z / r))*(y / r), -3 * (10 * pow(z / r, 2) - (35. / 3.)*pow(z / r, 4) - 1), temp);
		mult_as(1. / 2.*J3_EARTH*(mu / pow(r, 2))*pow(req / r, 3), temp, temp2);
		add_as(ajtot, temp2, ajtot);
	}
	if (num >= 4) {
		set3_as((3 - 42 * pow(z / r, 2) + 63 * pow(z / r, 4))*(x / r), (3 - 42 * pow(z / r, 2) + 63 * pow(z / r, 4))*(y / r), (15 - 70 * pow(z / r, 2) + 63 * pow(z / r, 4))*(z / r), temp);
		mult_as(5. / 8.*J4_EARTH*(mu / pow(r, 2))*pow(req / r, 4), temp, temp2);
		add_as(ajtot, temp2, ajtot);
	}
	if (num >= 5) {
		set3_as(3 * (35 * (z / r) - 210 * pow(z / r, 3) + 231 * pow(z / r, 5))*(x / r), 3 * (35 * (z / r) - 210 * pow(z / r, 3) + 231 * pow(z / r, 5))*(y / r), -(15 - 315 * pow(z / r, 2) + 945 * pow(z / r, 4) - 693 * pow(z / r, 6)), temp);
		mult_as(1. / 8.*J5_EARTH*(mu / pow(r, 2))*pow(req / r, 5), temp, temp2);
		add_as(ajtot, temp2, ajtot);
	}
	if (num >= 6) {
		set3_as((35 - 945 * pow(z / r, 2) + 3465 * pow(z / r, 4) - 3003 * pow(z / r, 6))*(x / r), (35 - 945 * pow(z / r, 2) + 3465 * pow(z / r, 4) - 3003 * pow(z / r, 6))*(y / r), -(3003 * pow(z / r, 6) - 4851 * pow(z / r, 4) + 2205 * pow(z / r, 2) - 245)*(z / r), temp);
		mult_as(-1. / 16.*J6_EARTH*(mu / pow(r, 2))*pow(req / r, 6), temp, temp2);
		add_as(ajtot, temp2, ajtot);
	}

	return;
}

/*
*  solarRad (A,m,sunvec,arvec)
*
*  Purpose:   Computes the inertial solar radiation force vectors
*             based on cross-sectional Area and mass of the spacecraft
*             and the position vector of the planet to the sun.
*             Note: It is assumed that the solar radiation pressure
*             quadratically with distance from sun (in AU)
*
*  Input is
*      A -      Cross-sectional area of the spacecraft that is facing
*               the sun in m^2.
*      m -      The mass of the spacecraft in kg.
*      sunvec - Position vector from the Sun to the orbiting Planet
*               in units of AU. Earth has a distance of 1 AU.
*
*  Output is
*      arvec  - The inertial acceleration vector due to the effects
*               of Solar Radiation pressure in km/sec^2.  The vector
*               components of the output are the same as the vector
*               components of the sunvec input vector.
*
*  Solar Radiation Equations obtained from
*  Earth Space and Planets Journal Vol. 51, 1999 pp. 979-986
*/
void solarRad(float A, float m, float *sunvec, float *arvec)
{
	float flux, c, Cr, sundist;

	/* Solar Radiation Flux */
	flux = 1372.5398; /* Watts/m^2 */

					  /* Speed of light */
	c = 2.997e8; /* m/s */

				 /* Radiation pressure coefficient */
	Cr = 1.3;

	/* Magnitude of position vector */
	sundist = norm_as(sunvec); /* AU */

							/* Computing the acceleration vector */
	mult_as((-Cr * A * flux) / (m * c * pow(sundist, 3)) / 1000., sunvec, arvec);

	return;
}

