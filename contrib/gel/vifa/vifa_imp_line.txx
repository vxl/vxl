// This is gel/vifa/vifa_imp_line.txx
#ifndef _VIFA_IMP_LINE_TXX_
#define _VIFA_IMP_LINE_TXX_

#include <vcl_cmath.h>  // for vcl_sqrt()
#include <vcl_cassert.h>
#include <vgl/vgl_line_2d.h>
#include <vifa/vifa_imp_line.h>


template <class Type> vifa_imp_line<Type>::
vifa_imp_line(vgl_point_2d<Type> const&  p1,
              vgl_point_2d<Type> const&  p2)
  : vgl_line_segment_2d<Type>(p1, p2)
{
  dx_ = p2.x() - p1.x();
  dy_ = p2.y() - p1.y();
}

template <class Type> vifa_imp_line<Type>::
vifa_imp_line(vgl_vector_2d<Type>  d,
              vgl_point_2d<Type>  m)
{
  dx_ = d.x();
  dy_ = d.y();

  vgl_point_2d<Type>  p1(m.x() - (dx_ / 2.0), m.y() - (dy_ / 2.0));
  vgl_point_2d<Type>  p2(m.x() + (dx_ / 2.0), m.y() + (dy_ / 2.0));
  this->set(p1, p2);
}

template <class Type> vifa_imp_line<Type>::
vifa_imp_line(Type  a,
              Type  b,
              Type  c
       )
{
  // The values of a and b should not both be zero
  assert (a || b);

  // Use implicit coefficients to compute two on-axis points
  vgl_point_2d<Type>  p1;
  vgl_point_2d<Type>  p2;
  vgl_line_2d<Type>  l(a, b, c);
  l.get_two_points(p1, p2);

  // Use the on-axis points for line segment endpoints
  set_points(p1, p2);
}

template <class Type> double vifa_imp_line<Type>::
get_dir_x(void)
{
  vgl_vector_2d<double>  v(dx_, dy_);
  double          s = v.length();

  s = (near_zero(s) ? 1.0 : 1.0 / s);

  double  dx = dx_ * s;
  return near_zero(dx) ? 0.0 : dx;
}

template <class Type> double vifa_imp_line<Type>::
length(void)
{
  vgl_vector_2d<double>  v = this->point2() - this->point1();
  double          s = v.length();

  return s;
}

template <class Type> double vifa_imp_line<Type>::
get_dir_y(void)
{
  vgl_vector_2d<double>  v(dx_, dy_);
  double          s = v.length();

  s = (near_zero(s) ? 1.0 : 1.0 / s);
  double  dy = dy_ * s;
  return near_zero(dy) ? 0.0 : dy;
}

template <class Type> void vifa_imp_line<Type>::
set_points(vgl_point_2d<Type> const&  p1,
           vgl_point_2d<Type> const&  p2)
{
  // Call base method to update the endpoints
  this->set(p1, p2);

  // Set axis projections of unit direction vector
  vgl_vector_2d<double>  d = this->direction();
  dx_ = d.x();
  dy_ = d.y();
}

template <class Type> void vifa_imp_line<Type>::
project_2d_pt(const Type& p,
              const Type& q,
              Type&       x,
              Type&       y) const
{
  double  a = this->a();
  double  b = this->b();
  double  c = this->c();
  double  a2 = a * a;
  double  b2 = b * b;
  double  m = vcl_sqrt(a2 + b2);

  a /= m;
  b /= m;
  c /= m;

  if (b != 0)
  {
    x = (Type)(- (a * c) + b2 * p + a * b * q);
    y = (Type)((- c + a2 * c - a * b2 * p + a2 * b * q) / b);
  }
  else
  {
    x = (Type)(- c / a);
    y = (Type)(q);
  }
}

template <class Type> vgl_point_2d<Type> vifa_imp_line<Type>::
project_2d_pt(const vgl_point_2d<Type>&  t) const
{
  Type  p = t.x();
  Type  q = t.y();
  Type  x;
  Type  y;

  project_2d_pt(p, q, x, y);

  vgl_point_2d<Type>  u(x, y);
  return u;
}

// Find parametric t-value for a given point relative to line segment.
template <class Type> double vifa_imp_line<Type>::
find_t(vgl_point_2d<Type> const&  p)
{
  double  dirx = get_dir_x();
  double  diry = get_dir_y();
  double  s = dx_ * dirx + dy_ * diry;

  if (near_zero(s))
  {
    return 0.5;
  }

  double  x = p.x() - this->point1().x();
  double  y = p.y() - this->point1().y();
  double  t = (x * dirx + y * diry) / s;
  return t;
}


// Find point on line (defined by line segment) for a parametric t-value.
template <class Type> vgl_point_2d<Type> vifa_imp_line<Type>::
find_at_t(double  t)
{
  Type  x = (Type)(this->point1().x() + dx_ * t);
  Type  y = (Type)(this->point1().y() + dy_ * t);

  vgl_point_2d<Type>  p(x, y);
  return p;
}


#endif  // _VIFA_IMP_LINE_TXX_
