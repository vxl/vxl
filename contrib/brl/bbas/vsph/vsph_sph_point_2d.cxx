// This is bbas/vsph/vsph_sph_point_2d.cxx
#include "vsph_sph_point_2d.h"
#include <vnl/vnl_math.h>
#include <vcl_cassert.h>

vsph_sph_point_2d::vsph_sph_point_2d(double theta, double phi, bool in_radians)
  : in_radians_(in_radians), theta_(theta),phi_(phi){
  double pye = in_radians?vnl_math::pi:180.0;
  double two_pye = 2.0*pye;
  assert(theta>=0.0);
  assert(theta<=pye);
  if (phi<-pye) phi_ = phi + two_pye;
  if (phi> pye) phi_ = phi - two_pye;
}

void vsph_sph_point_2d::print(vcl_ostream& os) const
{
  os << " vsph_sph_point_2d(";
  if (in_radians_) os << "rad"; else os << "deg";
  os << "):[theta=" << theta_ << ",phi=" << phi_ << "] ";
}

bool vsph_sph_point_2d::operator==(const vsph_sph_point_2d &other) const
{
  double tol = 0.0000001; //close enough for angles in radians
  double th = theta_, ph = phi_;
  if (!in_radians_) {
    th /= vnl_math::deg_per_rad;
    ph /= vnl_math::deg_per_rad;
  }
  double oth = other.theta_, oph = other.phi_;
  if (!other.in_radians_) {
    oth /= vnl_math::deg_per_rad;
    oph /= vnl_math::deg_per_rad;
  }
  double er = vcl_fabs(oth-th) + vcl_fabs(oph - ph);
  return er <= tol;
}

void vsph_sph_point_2d::b_read(vsl_b_istream& is)
{
  short version;
  vsl_b_read(is, version);
  switch (version) {
    case 1:
      vsl_b_read(is,in_radians_);
      vsl_b_read(is,theta_);
      vsl_b_read(is,phi_);
  }
}

void vsph_sph_point_2d::b_write(vsl_b_ostream& os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, in_radians_);
  vsl_b_write(os, theta_);
  vsl_b_write(os,phi_);
}

void vsl_b_read(vsl_b_istream& is, vsph_sph_point_2d& sp)
{
  sp.b_read(is);
}

void vsl_b_write(vsl_b_ostream& os, vsph_sph_point_2d const& sp)
{
  sp.b_write(os);
}

void vsl_print_summary(vcl_ostream& os, vsph_sph_point_2d const& sp)
{
  sp.print(os);
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_point_2d const& sp)
{
  sp.print(os);
  return os;
}

