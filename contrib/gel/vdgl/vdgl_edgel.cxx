// This is gel/vdgl/vdgl_edgel.cxx

#include <iostream>
#include "vdgl_edgel.h"
#include "vgl/vgl_point_2d.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

vdgl_edgel::vdgl_edgel( const double x, const double y, const double grad, const double theta )
  : p_( x, y), grad_( grad), theta_( theta) {}


bool operator==( const vdgl_edgel &e1, const vdgl_edgel &e2)
{
  return (( e1.p_.x()== e2.p_.x()) &&
          ( e1.p_.y()== e2.p_.y()) &&
          ( e1.grad_== e2.grad_) &&
          ( e2.theta_== e2.theta_));
}

std::ostream&  operator<<(std::ostream& s, const vdgl_edgel& p)
{
  return s << "<vdgl_edgel (" << p.p_.x() << ", " << p.p_.y() << "),("
           << p.grad_ << ", " << p.theta_ <<  ")>";
}
