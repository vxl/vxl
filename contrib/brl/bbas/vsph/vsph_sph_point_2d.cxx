// This is bbas/vsph/vsph_sph_point_2d.cxx
#include "vsph_sph_point_2d.h"


void vsph_sph_point_2d::print(vcl_ostream& os) const
{
  os << " vsph_sph_point_2d(";
  if(in_radians_) os << "rad"; else os << "deg";
  os << "):[theta=" << theta_ << ",phi=" << phi_ << "] ";
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
void vsl_b_read(vsl_b_istream& is, vsph_sph_point_2d& sp){
  sp.b_read(is);
}

void vsl_b_write(vsl_b_ostream& os, vsph_sph_point_2d const& sp){
  sp.b_write(os);
}


void vsl_print_summary(vcl_ostream& os, vsph_sph_point_2d const& sp){
  sp.print(os);
}
vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_point_2d const& sp)
{
  sp.print(os);
  return os;
}

