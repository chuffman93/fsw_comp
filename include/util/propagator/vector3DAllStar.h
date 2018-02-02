/*
*  vector3D.h
*
*  Created by Hanspeter Schaub on Sat Mar 01 2003.
*  Copyright (c) 2003 Hanspeter Schaub. All rights reserved.
*/

#ifndef _VECTOR_3D_ALLSTAR_H_
#define _VECTOR_3D_ALLSTAR_H_

void      set3_as(float v1, float v2, float v3, float result[4]);
void      setZero_as(float v[4]);
void      equal_as(float v1[4], float v2[4]);
void      add_as(float v1[4], float v2[4], float result[4]);
void      sub_as(float v1[4], float v2[4], float result[4]);
void      mult_as(float scaleFactor, float v[4], float result[4]);
void      cross_as(float v1[4], float v2[4], float result[4]);
float     dot_as(float v1[4], float v2[4]);
void      dotT_as(float v1[4], float v2[4], float result[4][4]);
float     norm_as(float v[4]);
int       equalCheck_as(float v1[4], float v2[4], float precisionExponent);
int       VequalCheck_as(float v1[4], float v2[4], float precisionExponent);
void      tilde_as(float v[4], float result[4][4]);
void      printVector_as(const char *str, float v[4]);

void      set4_as(float v1, float v2, float v3, float v4, float result[5]);
void      setZero4_as(float v[5]);
void      equal4_as(float v1[5], float v2[5]);
float     norm4_as(float v[4]);
void      printVector4_as(const char *str, float v[5]);
int       V4equalCheck_as(float v1[5], float v2[5], float precisionExponent);

void      setMatrix_as(float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33,
	float result[4][4]);
void      m33SetZero_as(float m[4][4]);
void      eye_as(float m[4][4]);
void      Madd_as(float m1[4][4], float m2[4][4], float m3[4][4]);
void      Msub_as(float m1[4][4], float m2[4][4], float m3[4][4]);
void      MdotM_as(float m1[4][4], float m2[4][4], float ans[4][4]);
void      Mdot_as(float m[4][4], float v[4], float result[4]);
void      MdotMT_as(float m1[4][4], float m2[4][4], float ans[4][4]);
void      Mmult_as(float, float m[4][4], float m2[4][4]);
void      transpose_as(float m[4][4], float ans[4][4]);
void      inverse_as(float m[4][4], float ans[4][4]);
float     detM_as(float a[4][4]);
float     trace_as(float m[4][4]);
void	  getColumn_as(float m[4][4], int col, float ans[4]);
void	  getRow_as(float m[4][4], int row, float ans[4]);
int       MequalCheck_as(float m1[4][4], float m2[4][4], float d);
void      printMatrix_as(const char *str, float m[4][4]);

float     arc_cosh_as(float);
float     arc_tanh_as(float);

#endif
