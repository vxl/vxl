#ifndef _HomgPoint2D_h
#define _HomgPoint2D_h
#ifdef __GNUC__
#pragma interface
#endif

//-*- c++ -*-------------------------------------------------------------
//
// .NAME HomgPoint2D - Homogeneous 3-vector for a 2D point
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgPoint2D.h
// .FILE HomgPoint2D.cxx
//
// .SECTION Description:
//
// A class to hold a homogeneous 3-vector for a 2D point.
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#include <vcl_iosfwd.h>
#include <vnl/vnl_double_2.h>
#include <mvl/Homg2D.h>

class HomgPoint2D : public Homg2D {
public:

  // Constructors/Initializers/Destructors-----------------------------------

  HomgPoint2D () {}
  HomgPoint2D (const HomgPoint2D& that): Homg2D(that) {}
  HomgPoint2D (double px, double py, double pw = 1.0): Homg2D(px,py,pw) {}
  HomgPoint2D (const vnl_vector<double>& vector_ptr): Homg2D(vector_ptr) {}

  HomgPoint2D& operator=(const HomgPoint2D& that)
  {
    Homg2D::operator=(that);
    return *this;
  }

  // Operations------------------------------------------------------------
  bool check_infinity(double tol = infinitesimal_tol) const;
  bool get_nonhomogeneous(double& px, double& py) const;
  vnl_double_2 get_double2() const;
  inline vnl_double_2 get_nonhomogeneous() const { return get_double2(); }

  HomgPoint2D get_unitized() const;

  // Utility Methods---------------------------------------------------------
  static HomgPoint2D read(istream&, bool is_homogeneous = false);
};

istream& operator>>(istream& is, HomgPoint2D& p);
ostream& operator<<(ostream& s, const HomgPoint2D& );

#endif // _HomgPoint2D_h
