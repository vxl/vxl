// This is core/vcsl/vsph_spherical_coord.cxx
#include "vsph_spherical_coord.h"
#include "vcl_complex.h"
#include "vcl_cmath.h"

#define RADIUS_THRESH 0.0001

vgl_point_3d<double> vsph_sph_point_3d::cart_coord() const
{
  double x,y,z;
  x = radius_*vcl_sin(theta_)*vcl_cos(phi_);
  y = radius_*vcl_sin(theta_)*vcl_sin(phi_);
  z = radius_*vcl_cos(theta_);
  // translate the point based on the origin
  vgl_point_3d<double> p(x,y,z);		
  return p;
}

void vsph_sph_point_3d::print(vcl_ostream& os) const 
{ 
  os << " vsph_sph_point_3d:[radius=" <<radius_ << ",theta=" << theta_ << ",phi=" << phi_ << "] "; 
}

vsph_spherical_coord::vsph_spherical_coord(vgl_point_3d<double> origin, double radius) 
: origin_(origin), radius_(radius)
{
}

void vsph_spherical_coord::spherical_coord(vgl_point_3d<double> cp, vsph_sph_point_3d& sp)
{
  double radius=0.0, theta=0.0, phi=0.0;

  radius = vcl_sqrt(cp.x()*cp.x()+cp.y()*cp.y()+cp.z()*cp.z());

  // if radius is zero, the rsult does not make sense
  if (vcl_abs(radius) < RADIUS_THRESH) {
	sp.set(0.0,0.0,0.0);
    return;
  }

  phi = vcl_atan2(cp.y(),cp.x());
  theta = vcl_acos(cp.z()/radius);
  sp.set(radius, theta, phi);
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_point_3d const& p) 
{ 
  p.print(os); 
  return os; 
}

vgl_point_3d<double> vsph_spherical_coord::cart_coord(vsph_sph_point_3d const& p) const
{ 
  vgl_point_3d<double> c = p.cart_coord();
  // translate the point from the origin
  double x = c.x()+origin_.x();
  double y = c.y()+origin_.y();
  double z = c.z()+origin_.z();
  vgl_point_3d<double> res(x, y, z);
  return res;
}

bool vsph_spherical_coord::move_point(vsph_sph_point_3d& p)
{
  // if it is already on the sphere
  if (vcl_abs(p.radius_ - radius_) < 0.01)
    return false;

  // create a new point with the right radius
  p.set(radius_, p.theta_, p.phi_);
  return true;
}

void vsph_spherical_coord::print(vcl_ostream& os) const 
{ 
  os << " vsph_spherical_coord:[radius=" <<radius_ << ", origin=" << origin_ << "] "; 
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_spherical_coord const& p) 
{ 
  p.print(os); 
  return os; 
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_spherical_coord_sptr const& p) 
{ 
  p->print(os); 
  return os; 
}