#include "bnl_simpson_integral.h"
#include <netlib/b3p_netlib.h>

double bnl_simpson_integral::int_fnct_(float* x)
{
  return  pfnct_->f_(*x);
}

double bnl_simpson_integral::integral(bnl_integrant_fnct* f, float a, float b, int n)
{

  float res = 0;
 
  //set the function
  pfnct_ = f;
  
  simpru_(&bnl_simpson_integral::int_fnct_, &a, &b, &n, &res);

  return res;
}
