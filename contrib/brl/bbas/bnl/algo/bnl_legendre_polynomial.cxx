#include "bnl_legendre_polynomial.h"
#include <vnl/vnl_real_polynomial.h>

vnl_real_polynomial bnl_legendre_polynomial(int n)
{
  if (n == 0 )
    return vnl_real_polynomial(1.0); // return 1
  else {
    if (n == 1) {
      // return p = x
      vnl_real_polynomial p(1);
      p[0] = 1;
      p[1] = 0;
      return p;
    }
    else {
      // p = x
      vnl_real_polynomial p(1);
      p[0] = 1; p[1] = 0;
      return (2 - 1.0/n) * p * bnl_legendre_polynomial(n-1)
           - vnl_real_polynomial((1 - 1.0/n)) * bnl_legendre_polynomial(n-2);
    }
  }
}


