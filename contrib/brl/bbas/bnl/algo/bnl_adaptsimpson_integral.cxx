#include "bnl_adaptsimpson_integral.h"
#include "bnl_netlib.h"

double bnl_adaptsimpson_integral::int_fnct_(float* x)
{
  return  pfnct_->f_(*x);
}

double bnl_adaptsimpson_integral::integral(bnl_integrant_fnct* f, float a, float b, float acury)
{

  float res = 0;
  float srmat[1111], errbnd;

  int m;
  int stat;
 
  //set the function
  pfnct_ = f;

  float tol = acury;
  
  adaptquad_(&bnl_adaptsimpson_integral::int_fnct_, &a, &b, &tol, srmat, &res, &errbnd, &m, &stat); 

  return res;
}
