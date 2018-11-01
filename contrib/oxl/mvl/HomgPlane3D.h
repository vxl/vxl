// This is oxl/mvl/HomgPlane3D.h
#ifndef HomgPlane3D_h_
#define HomgPlane3D_h_
//:
// \file
// \brief Homogeneous 4-vector for a 3D plane
//
// A class to hold a homogeneous 4-vector for a 3D plane.

#include <iostream>
#include <iosfwd>
#include <mvl/Homg3D.h>
#include <vnl/vnl_double_3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HomgPoint3D;

class HomgPlane3D : public Homg3D
{
 public:
  // Constructors/Initializers/Destructors-------------------------------------

  HomgPlane3D ();
  HomgPlane3D (double x, double y, double z, double w);
  HomgPlane3D (const vnl_double_3& n, double d);
  HomgPlane3D (const vnl_double_4& v): Homg3D(v) {}
  ~HomgPlane3D ();

  // Utility Methods-----------------------------------------------------------
  HomgPoint3D closest_point(const HomgPoint3D& x) const;
  double distance(const HomgPoint3D& x) const;

  vnl_vector<double> n() const { return homg_vector_.extract(3).normalize(); }
};

std::ostream& operator<<(std::ostream&, const HomgPlane3D&);

#endif // HomgPlane3D_h_
