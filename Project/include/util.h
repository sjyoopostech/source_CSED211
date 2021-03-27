#ifndef UTIL
#define UTIL

#define CONST_PIDOUBLE 6.2831853072
#define CONST_PI 3.1415629535
#define CONST_PIHALF 1.570796326

int clamp(int x, int a, int b);
void swap(int *x, int *y);
void order(int *x, int *y);

inline int absi(int x);

float floor(float x);
float mod(float x, float f);

float Q_rsqrt(float number);
float sqrt(float x);

float sin(float x);
float cos(float x);

#endif