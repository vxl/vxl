// This is oxl/mvl/HomgPlane3D.h
#ifndef HomgPlane3D_h_
#define HomgPlane3D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Homogeneous 4-vector for a 3D plane
//
// A class to hold a homogeneous 4-vector for a 3D plane.
//

#include <mvl/Homg3D.h>
#include <vcl_iosfwd.h>

class HomgPoint3D;

class HomgPlane3D : public Homg3D
{
 public:
  // Constructors/Initializers/Destructors-------------------------------------

  HomgPlane3D ();
  HomgPlane3D (double x, double y, double z, double w);
  HomgPlane3D (const vnl_vector<double>& n, double d);
  HomgPlane3D (const vnl_vector<double>& v): Homg3D(v) {}
  ~HomgPlane3D ();

  // Utility Methods-----------------------------------------------------------
  HomgPoint3D closest_point(const HomgPoint3D& x) const;
  double distance(const HomgPoint3D& x) const;

  vnl_vector<double> n() const { return _homg_vector.extract(3).normalize(); }
};

vcl_ostream& operator<<(vcl_ostream&, const HomgPlane3D&);

#endif // HomgPlane3D_h_
