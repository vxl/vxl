#ifndef pdf1d_resample_h
#define pdf1d_resample_h
//:
// \file
// \author Tim Cootes
// \brief Select random sample of data with replacement.

#include <vnl/vnl_vector.h>

//: Fill x with ns samples drawn at random from d
//  If ns not specified (or zero) then draw d.size() samples from d
void pdf1d_resample(vnl_vector<double>& x, const vnl_vector<double>& d, int ns=0);

//: Fill x with ns samples drawn at random from d[0..n-1]
//  If ns not specified (or zero) then draw d.size() samples from d
void pdf1d_resample(vnl_vector<double>& x, const double* d, int n, int ns=0);


#endif // pdf1d_resample_h
