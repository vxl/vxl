#include "vnl_simpson_integral.h"
#include <vnl/algo/vnl_netlib.h>

double vnl_simpson_integral::int_fnct_(float* x)
{
  return  pfnct_->f_(*x);
}

double vnl_simpson_integral::integral(vnl_integrant_fnct* f, float a, float b, int n)
{

  float res = 0;
 
  //set the function
  pfnct_ = f;
  
  simpru_(&vnl_simpson_integral::int_fnct_, &a, &b, &n, &res);

  return res;
}
