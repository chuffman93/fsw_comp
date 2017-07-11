/*
*  vector3D.h
*
*  Created by Hanspeter Schaub on Sat Mar 01 2003.
*  Copyright (c) 2003 Hanspeter Schaub. All rights reserved.
*/

#ifndef _VECTOR_3D_H_
#define _VECTOR_3D_H_

void      set3(float v1, float v2, float v3, float result[4]);
void      setZero(float v[4]);
void      equal(float v1[4], float v2[4]);
void      add(float v1[4], float v2[4], float result[4]);
void      sub(float v1[4], float v2[4], float result[4]);
void      mult(float scaleFactor, float v[4], float result[4]);
void      cross(float v1[4], float v2[4], float result[4]);
float     dot(float v1[4], float v2[4]);
void      dotT(float v1[4], float v2[4], float result[4][4]);
float     norm(float v[4]);
int       equalCheck(float v1[4], float v2[4], float precisionExponent);
int       VequalCheck(float v1[4], float v2[4], float precisionExponent);
void      tilde(float v[4], float result[4][4]);
void      printVector(const char *str, float v[4]);

void      set4(float v1, float v2, float v3, float v4, float result[5]);
void      setZero4(float v[5]);
void      equal4(float v1[5], float v2[5]);
float     norm4(float v[4]);
void      printVector4(const char *str, float v[5]);
int       V4equalCheck(float v1[5], float v2[5], float precisionExponent);

void      setMatrix(float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33,
	float result[4][4]);
void      m33SetZero(float m[4][4]);
void      eye(float m[4][4]);
void      Madd(float m1[4][4], float m2[4][4], float m3[4][4]);
void      Msub(float m1[4][4], float m2[4][4], float m3[4][4]);
void      MdotM(float m1[4][4], float m2[4][4], float ans[4][4]);
void      Mdot(float m[4][4], float v[4], float result[4]);
void      MdotMT(float m1[4][4], float m2[4][4], float ans[4][4]);
void      Mmult(float, float m[4][4], float m2[4][4]);
void      transpose(float m[4][4], float ans[4][4]);
void      inverse(float m[4][4], float ans[4][4]);
float     detM(float a[4][4]);
float     trace(float m[4][4]);
void	  getColumn(float m[4][4], int col, float ans[4]);
void	  getRow(float m[4][4], int row, float ans[4]);
int       MequalCheck(float m1[4][4], float m2[4][4], float d);
void      printMatrix(const char *str, float m[4][4]);

float     arc_cosh(float);
float     arc_tanh(float);

#endif
