#ifdef __GNUC__
#pragma implementation
#endif
#include "vnl_chi_squared.h"

// FORTRAN routine
extern "C" int chscdf_(float* x, int* nu, float* cdf);

// -- This subroutine computes the cumulative distribution function
// value for the chi-squared distribution with integer degrees of
// freedom parameter = dof.  This distribution is defined for all
// non-negative chisq.  Thus if a random variable x is drawn from a
// chi-squared distribution with d degrees of freedom, then P(x < X) =
// vnl_chi_squared::vnl_chi_squaredCumulative(X,d).
float vnl_chi_squared_cumulative(float chisq, int dof) {
  float cdf;
  chscdf_(&chisq,&dof,&cdf);
  return cdf;
}

//------------------------------------------------------------

double vnl_chi_squared_statistic_1 (int const *A, int const *B, int n, bool normalize)
{
  double sum = 0;
  
  if (normalize) {
    int sumA = 0;
    int sumB = 0;
    for (int i=0; i<n; ++i) {
      sumA += A[i];
      sumB += B[i];
    }
    
    for (int i=0; i<n; ++i)
      if (A[i]) {
	double a = double(A[i])/sumA;
	double b = double(B[i])/sumB;
	double tmp = a - b;
	sum += tmp*tmp/a;
      }
  }
  else {
    for (int i=0; i<n; ++i)
      if (A[i]) {
	double tmp = A[i] - B[i];
	sum += tmp*tmp/A[i];
      }
  }

  return sum;
}

double vnl_chi_squared_statistic_2 (int const *A, int const *B, int n, bool normalize)
{
  return vnl_chi_squared_statistic_1(B, A, n, normalize);
}

double vnl_chi_squared_statistic_12(int const *A, int const *B, int n, bool normalize)
{
  double sum = 0;
  
  if (normalize) {
    int sumA = 0;
    int sumB = 0;
    for (int i=0; i<n; ++i) {
      sumA += A[i];
      sumB += B[i];
    }
    
    for (int i=0; i<n; ++i)
      if (A[i] || B[i]) {
	double a = double(A[i])/sumA;
	double b = double(B[i])/sumB;
	double tmp = a - b;
	sum += tmp*tmp/(a + b);
      }
  }
  else {
    for (int i=0; i<n; ++i)
      if (A[i] || B[i]) {
	double tmp = A[i] - B[i];
	sum += tmp*tmp/(A[i] + B[i]);
      }
  }
  
  return sum;
}
