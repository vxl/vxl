#ifndef _HomgPoint3D_h
#define _HomgPoint3D_h
#ifdef __GNUC__
#pragma interface
#endif
//--------------------------------------------------------------
//
// .NAME HomgPoint3D - Homogeneous 4-vector representing a 3D point
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgPoint3D.h
// .FILE HomgPoint3D.cxx
//
// .SECTION Description:
// A class to hold a homogeneous 4-vector for a 3D point.
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#include <vnl/vnl_double_3.h>
#include <mvl/Homg3D.h>
#include <vcl/vcl_iosfwd.h>

class HomgPoint3D : public Homg3D {

  // PUBLIC INTERFACE--------------------------------------------------------
public:

  // Constructors/Initializers/Destructors-----------------------------------

  HomgPoint3D ();
  HomgPoint3D (const HomgPoint3D& p): Homg3D(p) {}
  HomgPoint3D (const vnl_vector<double>& p): Homg3D(p) {}
  HomgPoint3D (double x, double y, double z);
  HomgPoint3D (double x, double y, double z, double w);
  ~HomgPoint3D ();

  // Data Access-------------------------------------------------------------

  bool check_infinity() const;
  bool check_infinity(double tol) const;
  bool get_nonhomogeneous(double& x, double& y, double& z) const;
  vnl_double_3 get_double3() const;
  double radius() const { return get_double3().magnitude(); }
  double squared_radius() const { return get_double3().squared_magnitude(); }
};

ostream& operator<<(ostream& s, const HomgPoint3D& );

#endif // _HomgPoint3D_h
