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
// Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim
//

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <mvl/Homg3D.h>
#include <vcl_iosfwd.h>

class HomgPoint3D : public Homg3D
{
 public:
  // Constructors/Initializers/Destructors-----------------------------------

  HomgPoint3D ();
  HomgPoint3D (const HomgPoint3D& p): Homg3D(p) {}
  HomgPoint3D (const vnl_vector<double>& p): Homg3D(p) {}
  HomgPoint3D (const vnl_vector_fixed<double,4>& p): Homg3D(p) {}
  HomgPoint3D (double x, double y, double z);
  HomgPoint3D (double x, double y, double z, double w);
  ~HomgPoint3D ();

  // Data Access-------------------------------------------------------------

  bool check_infinity(double tol=1e-12) const;
  inline bool ideal(double tol = 1e-12) const { return check_infinity(tol); }
  bool get_nonhomogeneous(double& x, double& y, double& z) const;
  vnl_double_3 get_double3() const;
  double radius() const { return get_double3().magnitude(); }
  double squared_radius() const { return get_double3().squared_magnitude(); }
};

vcl_ostream& operator<<(vcl_ostream& s, const HomgPoint3D& );

#endif // HomgPoint3D_h_
