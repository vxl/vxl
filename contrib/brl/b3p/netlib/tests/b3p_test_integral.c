#include "../b3p_netlib.h"
#include <stdio.h>

double f(float *x)
{
  return (*x)/(1+(*x)*(*x));
}

void test_simpson_integral()
{
  float a = 0;
  float b = 1;
  float res;
  int n = 100;

  simpru_(&f, &a, &b, &n, &res);
  printf("simpson integral of x/(1+x^2) from 0 to 1 (%d grids) is %f\n", n, res);
}

void test_trapezod_integral()
{
  float a = 0;
  float b = 1;
  float res;
  int n = 100;

  trapru_(&f, &a, &b, &n, &res);
  printf("trapezod integral of x/(1+x^2) from 0 to 1 (%d grids) is %f\n", n, res);
}

void test_adapted_simpson_integral()
{
  float a = 0;
  float b = 1;
  float res;
  int n = 100;
  float rmat[1111];
  float tol = 1e-13;
  float errbound;
  int stat;

  adaptquad_(&f, &a, &b, &tol, rmat, &res, &errbound, &n, &stat);
  printf("adapted simpson integral (with tol=%g) of x/(1+x^2) from 0 to 1 (%d grids) is %f\n", tol, n,  res);
  printf("errbound is %f, state is %d\n", errbound,  stat);
}

int main()
{
  test_simpson_integral();
  test_trapezod_integral();
  test_adapted_simpson_integral();
  return 0;
}
