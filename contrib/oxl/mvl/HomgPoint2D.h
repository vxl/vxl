// This is oxl/mvl/HomgPoint2D.h
#ifndef HomgPoint2D_h_
#define HomgPoint2D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Homogeneous 3-vector for a 2D point
//
// A class to hold a homogeneous 3-vector for a 2D point.
//
// \verbatim
// Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim
//

#include <vcl_iosfwd.h>
#include <vnl/vnl_double_2.h>
#include <mvl/Homg2D.h>

class HomgPoint2D : public Homg2D
{
 public:
  // Constructors/Initializers/Destructors-----------------------------------

  HomgPoint2D () {}
  HomgPoint2D (const HomgPoint2D& that): Homg2D(that) {}
  HomgPoint2D (double px, double py, double pw = 1.0): Homg2D(px,py,pw) {}
  HomgPoint2D (const vnl_vector<double>& vector_ptr): Homg2D(vector_ptr) {}
  HomgPoint2D (const vnl_vector_fixed<double,3>& vector_ptr): Homg2D(vector_ptr) {}

  HomgPoint2D& operator=(const HomgPoint2D& that)
  {
    Homg2D::operator=(that);
    return *this;
  }

  // Operations------------------------------------------------------------
  bool check_infinity(double tol = 1e-12) const;
  inline bool ideal(double tol = 1e-12) const { return check_infinity(tol); }
  bool get_nonhomogeneous(double& px, double& py) const;
  vnl_double_2 get_double2() const;
  inline vnl_double_2 get_nonhomogeneous() const { return get_double2(); }

  HomgPoint2D get_unitized() const;

  // Utility Methods---------------------------------------------------------
  static HomgPoint2D read(vcl_istream&, bool is_homogeneous = false);
};

vcl_istream& operator>>(vcl_istream& is, HomgPoint2D& p);
vcl_ostream& operator<<(vcl_ostream& s, const HomgPoint2D& );

#endif // HomgPoint2D_h_
