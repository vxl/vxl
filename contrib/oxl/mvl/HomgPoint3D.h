// This is oxl/mvl/HomgPoint3D.h
#ifndef HomgPoint3D_h_
#define HomgPoint3D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Homogeneous 4-vector representing a 3D point
//
// A class to hold a homogeneous 4-vector for a 3D point.
//
// \verbatim
//  Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim

#include <vnl/vnl_double_3.h>
#include <mvl/Homg3D.h>
#include <vcl_iosfwd.h>

class HomgPoint3D : public Homg3D
{
 public:
  // Constructors/Initializers/Destructors-----------------------------------

  HomgPoint3D () {}
  HomgPoint3D (const HomgPoint3D& p): Homg3D(p) {}
  HomgPoint3D (const vnl_vector<double>& p): Homg3D(p) {}
  HomgPoint3D (const vnl_vector_fixed<double,4>& p): Homg3D(p) {}
  //: Constructor. The homogeneous parameter w defaults to 1.
  HomgPoint3D (double x, double y, double z, double w=1.0) : Homg3D(x,y,z,w) {}
 ~HomgPoint3D () {}

  // Data Access-------------------------------------------------------------

  //: Return true iff the point is the point at infinity.
  //  If tol == 0, w() must be exactly 0.
  //  Otherwise, tol is used as tolerance value (default: 1e-12),
  //  and $|w| <= \mbox{tol} \times min(|x|,|y|,|z|)$ is checked.
  inline bool ideal(double tol = 1e-12) const {
#define mvl_abs(x) ((x)<0?-(x):(x))
    return mvl_abs(w()) <= tol*mvl_abs(x()) && mvl_abs(w()) <= tol*mvl_abs(y()) && mvl_abs(w()) <= tol*mvl_abs(z());
#undef mvl_abs
  }

  bool get_nonhomogeneous(double& x, double& y, double& z) const;
  vnl_double_3 get_double3() const;
  double radius() const { return get_double3().magnitude(); }
  bool rescale_w(double new_w = 1.0);
  double squared_radius() const { return get_double3().squared_magnitude(); }
};

vcl_ostream& operator<<(vcl_ostream& s, const HomgPoint3D& );

#endif // HomgPoint3D_h_
