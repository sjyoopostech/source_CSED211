#include "util.h"

int clamp(int x, int a, int b) {
  x = (x < a ? a : x);
  x = (x > b ? b : x);
  return x;
}

void swap(int *x, int *y) {
	int tmp = *x;
	*x = *y;
	*y = tmp;
}

void order(int *x, int *y) {
	if (*x > *y)
		swap(x, y);
}

inline int absi(int x) {
	return x < 0 ? -x : x;
}

float floor(float x) {
	return (float)((int)x);
}

float mod(float x, float f) {
	x -= floor(x / f) * f;
	return x;
}

//Quake's inverse square root approximation
float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

float sqrt(float x) {
	return 1 / Q_rsqrt(x);
}

float sin(float x) {
	float x2, x3, x4, x5, x6;
	float d, n;

	x = mod(x + CONST_PI, CONST_PIDOUBLE) - CONST_PI;

	x2 = x*x;
	x3 = x2*x;
	x4 = x3*x;
	x5 = x4*x;
	x6 = x5*x;

	d = 0.00290358 * x5 - 0.129956 * x3 + x;
	n = 1.0 + 0.0367101 * x2 + 0.000688601 * x4 + 0.00000726192 * x6;

	return d/n;
}

float cos(float x) {
	return sin(x + CONST_PIHALF);
}