//:
// \file
#include "strk_epi_point.h"
#include <vcl_iostream.h>

// constructors
strk_epi_point::strk_epi_point(vgl_point_2d<double> &p,
                               const double alpha,
                               const double s,
                               const double grad_mag,
                               const double grad_ang,
                               const double tan_ang)
{
  p_ = p;
  alpha_ = alpha;
  s_ = s;
  grad_mag_ = grad_mag;
  grad_ang_ = grad_ang;
  tan_ang_ = tan_ang;
}

strk_epi_point::strk_epi_point(const double x, const double y,
                               const double alpha,
                               const double s,
                               const double grad_mag,
                               const double grad_ang,
                               const double tan_ang)
{
  p_.set(x, y);
  alpha_ = alpha;
  s_ = s;
  grad_mag_ = grad_mag;
  grad_ang_ = grad_ang;
  tan_ang_ = tan_ang;
}


strk_epi_point::strk_epi_point()
{
  this->set(0,0);
  alpha_ = 0;
  s_ = 0;
  grad_mag_ = -1;
  grad_ang_ = 0;
  tan_ang_ = 0;
}

vcl_ostream& operator<<(vcl_ostream& s, strk_epi_point const& ep)
{
  s << '(' << (int)ep.x() << ' ' << (int)ep.y() << ")[" << ep.alpha()
    << ' ' << ep.s() << ' '  << ep.grad_mag() << ' ' << ep.grad_ang() << ' '
    << ep.tan_ang() << '\n';

  return s;
}
