// This is mul/vpdfl/vpdfl_resample.cxx
#include "vpdfl_resample.h"
//:
// \file
// \author Tim Cootes
// \brief Select random sample of data with replacement.

#include <vnl/vnl_random.h>

// Object used to do sampling
static vnl_random vpdfl_resample_mz_random;

//: Fill x with n samples drawn at random from d
//  If n not specified (or zero) then draw d.size() samples from d
void vpdfl_resample(vcl_vector<vnl_vector<double> >& x,
                    const vcl_vector<vnl_vector<double> >& d, int ns)
{
  vpdfl_resample(x,&d[0],d.size(),ns);
}

//: Fill x with ns samples drawn at random from d[0..n-1]
//  If ns not specified (or zero) then draw d.size() samples from d
void vpdfl_resample(vcl_vector<vnl_vector<double> >& x,
                    const vnl_vector<double>* d, int n, int ns)
{
  if (ns==0) ns=n;
  x.resize(ns);

  for (int i=0;i<ns;++i)
    x[i] = d[vpdfl_resample_mz_random.lrand32(0,n-1)];
}
