// This is bbas/vsph/vsph_sph_point_3d.cxx
#include <complex>
#include <iostream>
#include <cmath>
#include "vsph_sph_point_3d.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define RADIUS_THRESH 0.0001

void vsph_sph_point_3d::print(std::ostream& os) const
{
  os << " vsph_sph_point_3d:[radius=" <<radius_ << ",theta=" << theta_ << ",phi=" << phi_ << "] ";
}

void vsph_sph_point_3d::b_read(vsl_b_istream& is)
{
  short version;
  vsl_b_read(is, version);
  switch (version) {
    case 1:
      vsl_b_read(is,radius_);
      vsl_b_read(is,theta_);
      vsl_b_read(is,phi_);
  }
}

void vsph_sph_point_3d::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, version());
  vsl_b_write(os, radius_);
  vsl_b_write(os, theta_);
  vsl_b_write(os,phi_);
}

std::ostream& operator<<(std::ostream& os, vsph_sph_point_3d const& p)
{
  p.print(os);
  return os;
}

void vsl_b_write(vsl_b_ostream& os, vsph_sph_point_3d& p)
{
  p.b_write(os);
}
void vsl_b_read(vsl_b_istream &is, vsph_sph_point_3d& p)
{
  p.b_read(is);
}
