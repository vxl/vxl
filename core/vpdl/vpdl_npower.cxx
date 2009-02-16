// This is core/vpdl/vpdl_npower.cxx

//:
// \file

#include "vpdl_npower.h"

//: initialize the first 8 powers of val
vpdl_npower_table::vpdl_npower_table(double val)
{
  values[0] = val; 
  for(unsigned int i=1; i<8; ++i)
    values[i] = values[i-1] * val;
}


//: return the nth power of 2*pi
// the first 8 are precomputed for common dimensions
double vpdl_npower_table::operator()(unsigned int n) const
{
  if(n == 0)
    return 1.0;
  if(n <= 8)
    return values[n-1];
  else
    return vpdl_npower<0>::power(values[0],n);
}

//: A static variable for precomputed powers of 2*pi
vpdl_npower_table vpdl_two_pi_power(2.0*vnl_math::pi);