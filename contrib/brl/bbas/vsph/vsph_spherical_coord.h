#ifndef vsph_spherical_coord_h_
#define vsph_spherical_coord_h_
//:
// \file
// \brief 3D spherical coordinate system
// \author Gamze Tunali
//
// \verbatim
//  Modifications
// \endverbatim

#include <iostream>
#include "vsph_spherical_coord_sptr.h"
#include "vsph_sph_point_3d.h"
#include <vgl/vgl_point_3d.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: 3D coordinate system specified by distance rho, angles theta (elevation) and phi (azimuth).
// theta is zero at the North Pole and 180 at the South Pole
// phi is zero pointing East, positive values rotating towards North

class vsph_spherical_coord : public vbl_ref_count
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  // Default constructor
  vsph_spherical_coord() : radius_(1.0), origin_(vgl_point_3d<double>(0,0,0)) {}

  vsph_spherical_coord(vgl_point_3d<double> origin, double radius = 1.0)
  : radius_(radius), origin_(origin) {}

  //: Copy constructor
  vsph_spherical_coord(vsph_spherical_coord const& rhs)
    : vbl_ref_count(), radius_(rhs.radius_), origin_(rhs.origin_) {}

  // Destructor
  ~vsph_spherical_coord() override = default;

  //***************************************************************************
  // Methods
  //***************************************************************************

  double radius() const { return radius_; }

  vgl_point_3d<double> origin() const { return origin_; }

  vsph_sph_point_3d create_point(double theta, double phi) const { return vsph_sph_point_3d(radius_,theta,phi); }

  void spherical_coord(vgl_point_3d<double> cp, vsph_sph_point_3d& sp);

  //: converts to cartesian coordinates
  vgl_point_3d<double> cart_coord(vsph_sph_point_3d const& p) const;

  //: converts to cartesian coordinates
  vgl_point_3d<double> cart_coord(double theta, double phi) const { return cart_coord(vsph_sph_point_3d(radius_,theta,phi)); }

  //: moves the point onto the surface of the sphere on the ray from origin to the point
  // Returns true if the point changed, false if it was already on the sphere
  bool move_point(vsph_sph_point_3d& p);

  void print(std::ostream& os) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os);

  short version() const { return 1; }

 private:
  double radius_;                // distance from the origin
  vgl_point_3d<double> origin_;  // the origin in cartesian coordinates
};

std::ostream& operator<<(std::ostream& os, vsph_spherical_coord const& p);
std::ostream& operator<<(std::ostream& os, vsph_spherical_coord_sptr const& p);

#endif // vsph_spherical_coord_h_
