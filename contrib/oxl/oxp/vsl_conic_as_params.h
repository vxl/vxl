#ifndef vsl_conic_as_params_h_
#define vsl_conic_as_params_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 12 Nov 99
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector_fixed.h>

//: 2D conic section, stored as a 6-vector [Axx Axy Ayy Ax Ay Ao]
struct vsl_conic_as_params : public vnl_vector_fixed<double, 6> {
  typedef vnl_vector_fixed<double, 6> base;

  vsl_conic_as_params(double cx, double cy, double rx, double ry, double theta);
  vsl_conic_as_params(const vnl_vector<double>& a) : base(a) {}
};

#endif // vsl_conic_as_params_h_
