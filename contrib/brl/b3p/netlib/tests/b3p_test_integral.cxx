#include <b3p_netlib.h>
#include <stdio.h>

double f(float *x)
{
  return (*x)/(1+(*x)*(*x));
}


#if 0
int b3p_test_integral(int argc, char * argv[])
#endif
  
int main()
{
  
  float a = 0;
  float b = 1;
  float res;

  int n = 100;

  simpru_(&f, &a, &b, &n, &res);
  printf("simpson integral of x/(1+x^2) from 0 to 1 (%d grids) is %lf \n", n, res);

  trapru_(&f, &a, &b, &n, &res);
  printf("trapezod integral of x/(1+x^2) from 0 to 1 (%d grids) is %lf \n", n, res);


  double rvec[11];
  float rmat[1111];
  float tol = 1e-13;
  float errbound;
  int stat;
  adaptquad_(&f, &a, &b, &tol, rmat, &res, &errbound, &n, &stat );
  printf("adapted simpson integral of x/(1+x^2) from 0 to 1 (%d grids)  is %lf \n", n,  res);
  return 0;
}
