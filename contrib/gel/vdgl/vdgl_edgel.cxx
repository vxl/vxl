// This is gel/vdgl/vdgl_edgel.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vdgl_edgel.h"
#include <vcl_iostream.h>

vdgl_edgel::vdgl_edgel( const double x, const double y, const double grad, const double theta )
  : p_( x, y), grad_( grad), theta_( theta)
{
}

vdgl_edgel& vdgl_edgel::operator=(const vdgl_edgel& that)
{
  p_    = vgl_point_2d<double>( that.get_x(), that.get_y());
  grad_ = that.get_grad();
  theta_= that.get_theta();

  return *this;
}


bool operator==( const vdgl_edgel &e1, const vdgl_edgel &e2)
{
  return (( e1.p_.x()== e2.p_.x()) &&
          ( e1.p_.y()== e2.p_.y()) &&
          ( e1.grad_== e2.grad_) &&
          ( e2.theta_== e2.theta_));
}

vcl_ostream&  operator<<(vcl_ostream& s, const vdgl_edgel& p)
{
  return s << "<vdgl_edgel (" << p.p_.x() << ", " << p.p_.y() << "),("
           << p.grad_ << ", " << p.theta_ <<  ")>";
}

