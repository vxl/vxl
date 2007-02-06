// This is core/vnl/algo/vnl_bracket_minimum.h
#ifndef vnl_bracket_minimum_h_
#define vnl_bracket_minimum_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Function to bracket a minimum
// \author Tim Cootes
// \date   Feb 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <vnl/vnl_cost_function.h>

//: Given initial values a and b, find bracket a<b<c s.t. f(a)>f(b)<f(c)
//  Final function values at a,b,c stored in fa,fb,fc.
//
//  This does the simplest possible thing, taking steps of increasing
//  size in the downhill direction.  More advanced versions might use
//  parabolic prediction.
//  Note that there's currently nothing
//  to stop it if supplied with a monotonic function - it will just continue
//  forever.
void vnl_bracket_minimum(vnl_cost_function& f,
                        double& a, double& b, double& c,
                        double& fa, double& fb, double& fc);

#endif // vnl_bracket_minimum_h_
