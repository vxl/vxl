// This is oxl/mvl/HomgPoint1D.h
#ifndef HomgPoint1D_h_
#define HomgPoint1D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief Homogeneous 1D point
//
// HomgPoint1D represents a 1D point in homogeneous coordinates
//
// \author Peter Vanroose
// \date   13 October 2002
//
// \verbatim
//  Modifications:
//     Peter Vanroose - 13 Oct 2002 - now using vgl_homg_point_1d<double>
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vgl/vgl_homg_point_1d.h>

class HomgPoint1D : public vgl_homg_point_1d<double>
{
 public:

  // constructors.
  HomgPoint1D() : vgl_homg_point_1d<double>() {}
  HomgPoint1D(double px, double pw=1.0) : vgl_homg_point_1d<double>(px,pw) {}

  // Operations----------------------------------------------------------------

 private:
  //: @{ Check $|w| < \mbox{tol} \times |x|$. @}
  // Deprecated name; use ideal() instead.
  bool check_infinity(double tol = 1e-12) const { return ideal(tol); }
};

#endif // HomgPoint1D_h_
