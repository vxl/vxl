// This is mul/pdf1d/pdf1d_resample.cxx
#include "pdf1d_resample.h"
//:
// \file
// \author Tim Cootes
// \brief Select random sample of data with replacement.

#include <vnl/vnl_random.h>

// Object used to do sampling
static vnl_random pdf1d_resample_mz_random;

//: Fill x with n samples drawn at random from d
//  If n not specified (or zero) then draw d.size() samples from d
void pdf1d_resample(vnl_vector<double>& x, const vnl_vector<double>& d, int ns)
{
  pdf1d_resample(x,d.data_block(),d.size(),ns);
}

//: Fill x with ns samples drawn at random from d[0..n-1]
//  If ns not specified (or zero) then draw d.size() samples from d
void pdf1d_resample(vnl_vector<double>& x, const double* d, int n, int ns)
{
  if (ns==0) ns=n;
  x.set_size(ns);

  for (int i=0;i<ns;++i)
    x[i] = d[pdf1d_resample_mz_random.lrand32(0,n-1)];
}
