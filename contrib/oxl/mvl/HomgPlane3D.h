#ifndef _HomgPlane3D_h
#define _HomgPlane3D_h
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//
// .NAME HomgPlane3D - Homogeneous 4-vector for a 3D plane
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgPlane3D.h
// .FILE HomgPlane3D.cxx
//
// .SECTION Description:
// A class to hold a homogeneous 4-vector for a 3D plane.
//

#include <mvl/Homg3D.h>
#include <vcl_iosfwd.h>

class HomgPoint3D;

class HomgPlane3D : public Homg3D {

  // PUBLIC INTERFACE----------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-------------------------------------

  HomgPlane3D ();
  HomgPlane3D (double x, double y, double z, double w);
  HomgPlane3D (const vnl_vector<double>& n, double d);
  HomgPlane3D (const vnl_vector<double>& v): Homg3D(v) {}
  ~HomgPlane3D ();

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

  // Utility Methods-----------------------------------------------------------
  HomgPoint3D closest_point(const HomgPoint3D& x) const;
  double distance(const HomgPoint3D& x) const;

  vnl_vector<double> n() const { return _homg_vector.extract(3).normalize(); }

  // Derived operator== is now working so this is not needed:
  //bool operator==(HomgPlane3D const& p) const { return Homg3D::operator==(p); }

  // INTERNALS-----------------------------------------------------------------

protected:

private:

  // Data Members--------------------------------------------------------------

private:

};

vcl_ostream& operator<<(vcl_ostream&, const HomgPlane3D&);

#endif // _HomgPlane3D_h
