#include <b3p_netlib.h>
#include <stdio.h>

double f(float *x)
{
  return (*x)/(1+(*x)*(*x));
}



//int b3p_test_simpsonint(int argc, char * argv[])
int main(int argc, char* argv[])
{
  
  float a = 0;
  float b = 1;
  float res;

  int n = 100;

  simpru_(&f, &a, &b, &n, &res);
  printf("simpson integration of x/(1+x^2) from 0 to 1 is %f \n", res);

  trapru_(&f, &a, &b, &n, &res);
  printf("trapezod integration of x/(1+x^2) from 0 to 1 is %f \n", res);
  return 0;
}
