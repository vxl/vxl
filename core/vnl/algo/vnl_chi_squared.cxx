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
