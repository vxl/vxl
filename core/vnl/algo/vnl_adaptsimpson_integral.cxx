#include "vnl_adaptsimpson_integral.h"
#include <vnl/algo/vnl_netlib.h>

double vnl_adaptsimpson_integral::int_fnct_(float* x)
{
  return  pfnct_->f_(*x);
}

double vnl_adaptsimpson_integral::integral(vnl_integrant_fnct* f, float a, float b, float acury)
{

  float res = 0;
  float srmat[1111], errbnd;

  int m;
  int stat;
 
  //set the function
  pfnct_ = f;

  float tol = acury;
  
  adaptquad_(&vnl_adaptsimpson_integral::int_fnct_, &a, &b, &tol, srmat, &res, &errbnd, &m, &stat); 

  return res;
}
