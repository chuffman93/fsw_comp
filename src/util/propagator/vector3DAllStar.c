/*
*  vector3D.c
*
*  Created by Hanspeter Schaub on Sat Mar 01 2003.
*  Copyright (c) 2003 Hanspeter Schaub. All rights reserved.
*
*  Provides a library for doing 3D matrix algebra.
*/

#include "util/propagator/vector3DAllStar.h"

#include <math.h>
#include <stdio.h>

void set3_as(float v1, float v2, float v3, float result[4])
{
	result[1] = v1;
	result[2] = v2;
	result[3] = v3;
}

void setZero_as(float v[4])
{
	v[1] = 0.;
	v[2] = 0.;
	v[3] = 0.;
}

void equal_as(float v1[4], float v2[4])
{
	v2[1] = v1[1];
	v2[2] = v1[2];
	v2[3] = v1[3];
}

void add_as(float v1[4], float v2[4], float result[4])
{
	result[1] = v1[1] + v2[1];
	result[2] = v1[2] + v2[2];
	result[3] = v1[3] + v2[3];
}

void sub_as(float v1[4], float v2[4], float result[4])
{
	result[1] = v1[1] - v2[1];
	result[2] = v1[2] - v2[2];
	result[3] = v1[3] - v2[3];
}

void mult_as(float scaleFactor, float v[4], float result[4])
{
	result[1] = scaleFactor * v[1];
	result[2] = scaleFactor * v[2];
	result[3] = scaleFactor * v[3];
}

void cross_as(float v1[4], float v2[4], float result[4])
{
	result[1] = v1[2] * v2[3] - v1[3] * v2[2];
	result[2] = v1[3] * v2[1] - v1[1] * v2[3];
	result[3] = v1[1] * v2[2] - v1[2] * v2[1];
}

float dot_as(float v1[4], float v2[4])
{
	return v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}

void dotT_as(float v1[4], float v2[4], float result[4][4])
{
	mult_as(v1[1], v2, result[1]);
	mult_as(v1[2], v2, result[2]);
	mult_as(v1[3], v2, result[3]);
}

float norm_as(float v[4])
{
	return sqrt(v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

int equalCheck_as(float v1[4], float v2[4], float precisionExponent)
{
	int c = 1;
	int i;

	for (i = 1; i <= 3; i++) {
		if (fabs(v1[i] - v2[i]) > pow(10., -precisionExponent)) {
			c = 0;
		}
	}

	return c;
}

int VequalCheck_as(float v1[4], float v2[4], float precisionExponent)
{
	int c = 1;
	int i;

	for (i = 1; i <= 3; i++) {
		if (fabs(v1[i] - v2[i]) > pow(10., -precisionExponent)) {
			c = 0;
		}
	}

	return c;
}

void tilde_as(float v[4], float result[4][4])
{
	result[1][1] = result[2][2] = result[3][3] = 0.;
	result[1][2] = -v[3];
	result[2][1] = v[3];
	result[1][3] = v[2];
	result[3][1] = -v[2];
	result[2][3] = -v[1];
	result[3][2] = v[1];
}

void printVector_as(const char *str, float v[4])
{
	printf("%s (%20.15g, %20.15g, %20.15g)\n", str, v[1], v[2], v[3]);
}

void set4_as(float v1, float v2, float v3, float v4, float result[5])
{
	result[1] = v1;
	result[2] = v2;
	result[3] = v3;
	result[4] = v4;
}

void setZero4_as(float v[5])
{
	v[1] = 0.0;
	v[2] = 0.0;
	v[3] = 0.0;
	v[4] = 0.0;
}

void equal4_as(float v1[5], float v2[5])
{
	v2[1] = v1[1];
	v2[2] = v1[2];
	v2[3] = v1[3];
	v2[4] = v1[4];
}

float norm4_as(float v[5])
{
	float result;

	result = sqrt(v[1] * v[1] + v[2] * v[2] + v[3] * v[3] + v[4] * v[4]);

	return result;
}

void printVector4_as(const char *str, float v[5])
{
	printf("%s (%20.15g, %20.15g, %20.15g, %20.15g)\n",
		str, v[1], v[2], v[3], v[4]);
}

int V4equalCheck_as(float v1[5], float v2[5], float precisionExponent)
{
	int c = 1;
	int i;

	for (i = 1; i <= 4; i++) {
		if (fabs(v1[i] - v2[i]) > pow(10., -precisionExponent)) {
			c = 0;
		}
	}

	return c;
}

void setMatrix_as(float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33, float result[4][4])
{
	result[1][1] = m11;
	result[1][2] = m12;
	result[1][3] = m13;
	result[2][1] = m21;
	result[2][2] = m22;
	result[2][3] = m23;
	result[3][1] = m31;
	result[3][2] = m32;
	result[3][3] = m33;
}

void m33SetZero_as(float m[4][4])
{
	int i;
	int j;
	for (i = 1; i < 3 + 1; i++) {
		for (j = 1; j < 3 + 1; j++) {
			m[i][j] = 0.0;
		}
	}
}

void eye_as(float m[4][4])
{
	m[1][1] = 1.0;
	m[1][2] = 0.0;
	m[1][3] = 0.0;
	m[2][1] = 0.0;
	m[2][2] = 1.0;
	m[2][3] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0;
}

void Madd_as(float m1[4][4], float m2[4][4], float m3[4][4])
{
	add_as(m1[1], m2[1], m3[1]);
	add_as(m1[2], m2[2], m3[2]);
	add_as(m1[3], m2[3], m3[3]);
}

void Msub_as(float m1[4][4], float m2[4][4], float m3[4][4])
{
	sub_as(m1[1], m2[1], m3[1]);
	sub_as(m1[2], m2[2], m3[2]);
	sub_as(m1[3], m2[3], m3[3]);
}

/* Multiplication of two matrices. M1*M2 */
void MdotM_as(float m1[4][4], float m0[4][4], float ans[4][4])
{
	float m2[4][4];
	transpose_as(m0, m2);

	ans[1][1] = dot_as(m1[1], m2[1]);
	ans[1][2] = dot_as(m1[1], m2[2]);
	ans[1][3] = dot_as(m1[1], m2[3]);
	ans[2][1] = dot_as(m1[2], m2[1]);
	ans[2][2] = dot_as(m1[2], m2[2]);
	ans[2][3] = dot_as(m1[2], m2[3]);
	ans[3][1] = dot_as(m1[3], m2[1]);
	ans[3][2] = dot_as(m1[3], m2[2]);
	ans[3][3] = dot_as(m1[3], m2[3]);
}

/* Multiplication between Matrix and vector. M*v */
void Mdot_as(float m[4][4], float v[4], float result[4])
{
	result[1] = dot_as(m[1], v);
	result[2] = dot_as(m[2], v);
	result[3] = dot_as(m[3], v);
}

void MdotMT_as(float m1[4][4], float m0[4][4], float ans[4][4])
{
	ans[1][1] = dot_as(m1[1], m0[1]);
	ans[1][2] = dot_as(m1[1], m0[2]);
	ans[1][3] = dot_as(m1[1], m0[3]);
	ans[2][1] = dot_as(m1[2], m0[1]);
	ans[2][2] = dot_as(m1[2], m0[2]);
	ans[2][3] = dot_as(m1[2], m0[3]);
	ans[3][1] = dot_as(m1[3], m0[1]);
	ans[3][2] = dot_as(m1[3], m0[2]);
	ans[3][3] = dot_as(m1[3], m0[3]);
}

void Mmult_as(float a, float m[4][4], float m2[4][4])
{
	mult_as(a, m[1], m2[1]);
	mult_as(a, m[2], m2[2]);
	mult_as(a, m[3], m2[3]);
}

void transpose_as(float m1[4][4], float ans[4][4])
{
	ans[1][1] = m1[1][1];
	ans[1][2] = m1[2][1];
	ans[1][3] = m1[3][1];
	ans[2][1] = m1[1][2];
	ans[2][2] = m1[2][2];
	ans[2][3] = m1[3][2];
	ans[3][1] = m1[1][3];
	ans[3][2] = m1[2][3];
	ans[3][3] = m1[3][3];
}
void inverse_as(float m[4][4], float ans[4][4])
{
	float det;

	det = detM_as(m);

	//if(fabs(det) > 1e-15) {
	ans[1][1] = (-m[2][3] * m[3][2] + m[2][2] * m[3][3]) / det;
	ans[1][2] = (m[1][3] * m[3][2] - m[1][2] * m[3][3]) / det;
	ans[1][3] = (-m[1][3] * m[2][2] + m[1][2] * m[2][3]) / det;
	ans[2][1] = (m[2][3] * m[3][1] - m[2][1] * m[3][3]) / det;
	ans[2][2] = (-m[1][3] * m[3][1] + m[1][1] * m[3][3]) / det;
	ans[2][3] = (m[1][3] * m[2][1] - m[1][1] * m[2][3]) / det;
	ans[3][1] = (-m[2][2] * m[3][1] + m[2][1] * m[3][2]) / det;
	ans[3][2] = (m[1][2] * m[3][1] - m[1][1] * m[3][2]) / det;
	ans[3][3] = (-m[1][2] * m[2][1] + m[1][1] * m[2][2]) / det;
	//} else {
	//    fprintf(stderr, "ERROR: singular 3x3 matrix inverse\n");
	//    ans[1][1] = ans[1][2] = ans[1][3] = ans[2][1] = ans[2][2] = ans[2][3] =
	//                ans[3][1] = ans[3][2] = ans[3][3] = NAN;
	//}
}

float detM_as(float m[4][4])
{
	return -m[1][3] * m[2][2] * m[3][1] + m[1][2] * m[2][3] * m[3][1]
		+ m[1][3] * m[2][1] * m[3][2] - m[1][1] * m[2][3] * m[3][2]
		- m[1][2] * m[2][1] * m[3][3] + m[1][1] * m[2][2] * m[3][3];
}

float trace_as(float m[4][4])
{
	return m[1][1] + m[2][2] + m[3][3];
}

void getColumn_as(float m[4][4], int col, float ans[4])
{
	ans[1] = m[1][col];
	ans[2] = m[2][col];
	ans[3] = m[3][col];
}

void getRow_as(float m[4][4], int row, float ans[4])
{
	ans[1] = m[row][1];
	ans[2] = m[row][2];
	ans[3] = m[row][3];
}

int MequalCheck_as(float m1[4][4], float m2[4][4], float d)
{
	int c = 1;
	int i;
	int j;

	for (i = 1; i <= 3; i++) {
		for (j = 1; j <= 3; j++) {
			if (fabs(m1[i][j] - m2[i][j]) > pow(10., -d)) {
				c = 0;
			}
		}
	}

	return c;
}

void printMatrix_as(const char *str, float m[4][4])
{
	printf("%s:\n%20.15g, %20.15g, %20.15g\n", str, m[1][1], m[1][2], m[1][3]);
	printf("%20.15g, %20.15g, %20.15g\n", m[2][1], m[2][2], m[2][3]);
	printf("%20.15g, %20.15g, %20.15g\n", m[3][1], m[3][2], m[3][3]);
}

float arc_cosh_as(float z)
{
	return log(z + sqrt(z + 1.) * sqrt(z - 1.));
}

float arc_tanh_as(float z)
{
	return 0.5 * (log(1. + z) - log(1. - z));
}
