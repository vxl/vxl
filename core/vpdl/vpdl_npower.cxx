// This is core/vpdl/vpdl_npower.cxx
#include "vpdl_npower.h"
//:
// \file


#include <vnl/vnl_math.h>
#include <vcl_cassert.h>


//: initialize the first \a num powers of \a val
vpdl_npower_table::vpdl_npower_table(double val, unsigned int num)
: num_v_(num+1), values_(new double[num_v_])
{
  assert(num_v_ > 0);
  values_[0] = 1.0;
  for (unsigned int i=1; i<num_v_; ++i)
    values_[i] = values_[i-1] * val;
}

//: destructor
vpdl_npower_table::~vpdl_npower_table()
{
  delete [] values_;
}

//: return the nth power of the value in the table
// the first \c num are precomputed for common dimensions
double vpdl_npower_table::operator()(unsigned int n) const
{
  if (n < num_v_)
    return values_[n];
  else
  {
    double val = values_[num_v_-1];
    for(unsigned int i=num_v_-1; i<n; ++i)
      val *= values_[1];
    return val;
  }
}


//: A static variable for precomputed powers of 2*pi
vpdl_npower_table vpdl_two_pi_power(2.0*vnl_math::pi);
