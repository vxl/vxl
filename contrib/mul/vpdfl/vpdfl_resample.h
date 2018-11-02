#ifndef vpdfl_resample_h
#define vpdfl_resample_h
//:
// \file
// \author Tim Cootes
// \brief Select random sample of data with replacement.

#include <iostream>
#include <vector>
#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Fill x with ns samples drawn at random from d
//  If ns not specified (or zero) then draw d.size() samples from d
void vpdfl_resample(std::vector<vnl_vector<double> >& x,
                    const std::vector<vnl_vector<double> >& d, int ns=0);

//: Fill x with ns samples drawn at random from d[0..n-1]
//  If ns not specified (or zero) then draw d.size() samples from d
void vpdfl_resample(std::vector<vnl_vector<double> >& x,
                    const vnl_vector<double>* d, int n, int ns=0);


#endif // vpdfl_resample_h
