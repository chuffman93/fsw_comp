/*
 *  gpsFrameRotation.h
 *
 *  Created by Lee Jasper on Friday April 5, 2013.
 *  Copyright (c) 2013 Lee Jasper. All rights reserved.
 *
 *  Original MatLab software provided by Ben K. Bradley
 *  This is simply a conversion of Ben's code into FSW
 *   for the CICERO project.
 *  University of Colorado, Boulder
 *
 */

#ifndef _GPS_FRAME_ROTATION_H_
#define _GPS_FRAME_ROTATION_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "vector3D.h"
//#include "ciceroDefinitions.h"
#include "IAU2006_XYS.h"
#include "leapSecond.h"
//#include "RigidBodyKinematics.h"

//const double OMEGA_EARTH = 7.29211585530066e-5; //rad/s Rotation Rate of Earth

typedef struct {
    double    *array;
    int         used;
    int         size;
} fpArray;

typedef struct {
    double    **array;
    int         used;
    int         sizeX;
    int         sizeY;
} fpDoubleArray;

void wgs2gcrf(double *xEcef, double *vEcef, double *gpsTime, double *xEci, double *vEci);
void gcrf2wgs(double *xEci, double *vEci, double *gpsTime, double *xEcef, double *vEcef);
void gpstow2jd( int32_t wn, double tow, int rollflag, double *jd_gps );
void format_JD( double *jd_orig, double *jd );
void jdtt2jdutc( double *jd_tt, double *jd_utc, int *tai_utc );
void ecef2eci_IAU2006CIOinterp( double *jd_tt, double *jd_utc, double *r0, double *r, double *v0, double *v, double IE[3][3] );
void eci2ecef_IAU2006CIOinterp(double *jd_tt, double *jd_utc, double *r0, double *r, double *v0, double *v, double EI[3][3]);
void computeERAmat( double *jd_ut1, double rERA[3][3], double *ERA );
void getXYs_simple( double mjd_tt, double *X, double *Y, double *s );
void interpLagrange( double xx, int p, int row0, double *yy);
void computeBPNmatrix( double X, double Y, double s, double BPN[3][3]);

void gcrf2j2000(double *xEci, double *vEci, double *xJ2000, double *vJ2000, double B[4][4]);
void j20002gcrf(double *xJ2000, double *vJ2000, double *xEci, double *vEci, double B[4][4]);

void initArray(fpArray *a, int initialSize);
void insertArray(fpArray *a, double element);
void freeArray(fpArray *a);
void initDoubleArray(fpDoubleArray *a, int initialDim1, int initialDim2);
void Mult1xN_NxM( fpArray *a, fpDoubleArray *b, double *c );
void freeDoubleArray(fpDoubleArray *a, int dim1);



#endif
