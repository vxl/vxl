// This is core/vgl/vgl_infinite_line_3d.hxx
#ifndef vgl_infinite_line_3d_hxx_
#define vgl_infinite_line_3d_hxx_

#include <iostream>
#include <cmath>
#include <string>
#include "vgl_infinite_line_3d.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
template <class Type>
vgl_infinite_line_3d<Type>::vgl_infinite_line_3d(vgl_point_3d<Type> const& p1,
                                                 vgl_point_3d<Type> const& p2)
{
  vgl_vector_3d<Type> dir = p2-p1;
  vgl_infinite_line_3d<Type> l(p1, dir);
  x0_ = l.x0();
  t_ = dir;
}

template <class Type>
void vgl_infinite_line_3d<Type>::
compute_uv_vectors(vgl_vector_3d<Type>& u, vgl_vector_3d<Type>& v) const
{
  // define the plane coordinate system (u, v)
  // v is given by the cross product of t with x, unless t is nearly
  // parallel to x, in which case v is given by z X t.
  vgl_vector_3d<Type> x(Type(1), Type(0), Type(0));
  v = cross_product(t_,x);
  Type vmag = static_cast<Type>(v.length());
  double vmagd = static_cast<double>(vmag);
  if (vmagd < 1.0e-8) {
    vgl_vector_3d<Type> z(Type(0), Type(0), Type(1));
    v = cross_product(z, t_);
    vmag = static_cast<Type>(v.length());
    assert(vmag>Type(0));
    v/=vmag;
  }
  else v/=vmag;
  // The other plane coordinate vector is perpendicular to both t and v
  u = cross_product(v,t_);
  Type umag = static_cast<Type>(u.length());
  u/=umag;
}

template <class Type>
vgl_infinite_line_3d<Type>::
vgl_infinite_line_3d(vgl_point_3d<Type> const& p,
                     vgl_vector_3d<Type> const& dir)
{
  // reconcile direction so that tangent is in the positive hemisphere
  double ttx = std::fabs(static_cast<double>(dir.x()));
  double tty = std::fabs(static_cast<double>(dir.y()));
  double ttz = std::fabs(static_cast<double>(dir.z()));
  double max_comp = ttx;
  double sign = static_cast<double>(dir.x());
  if (max_comp < tty) {
    max_comp = tty;
    sign = static_cast<double>(dir.y());
  }
  if (max_comp < ttz) {
    max_comp = ttz;
    sign = static_cast<double>(dir.z());
  }
  // switch sense if max component is negative
  Type sense = static_cast<Type>(sign/max_comp);
  t_ = normalized(dir*sense);
  // Define the plane perpendicular to the line passing through the origin
  // the plane normal is t_ the distance of the plane from the origin is 0
  // it follows that the intersection of the line with the perpendicular plane
  // is as follows:
  Type mag = static_cast<Type>(t_.length());
  assert(mag>Type(0));
  vgl_vector_3d<Type> pv(p.x(), p.y(), p.z());
  Type dp = dot_product(pv, t_);
  Type k = -dp/(mag*mag);
  // The intersection point
  vgl_vector_3d<Type> p0 = pv + k*t_, u, v;
  this->compute_uv_vectors(u, v);
  // The location of the intersection point in plane coordinates can now be computed
  Type u0 = dot_product(u, p0), v0 = dot_product(v, p0);
  x0_.set(u0, v0);
}

// the point on the line closest to the origin
template <class Type>
vgl_point_3d<Type> vgl_infinite_line_3d<Type>::point() const
{
  // u,v plane coordinate vectors
  vgl_vector_3d<Type> u, v, pv;
  this->compute_uv_vectors(u, v);
  pv = x0_.x()*u + x0_.y()*v;
  return vgl_point_3d<Type>(pv.x(), pv.y(), pv.z());
}

template <class Type>
bool vgl_infinite_line_3d<Type>::contains(const vgl_point_3d<Type>& p ) const
{
  vgl_point_3d<Type> point1 = this->point();
  vgl_point_3d<Type> point2 = this->point_t(Type(1));
  double seg = 1.0;
  double len1 = static_cast<double>((point1 - p).length());
  double len2 = static_cast<double>((point2 - p).length());
  // two cases: point inside (point1, point2) segment;
  //            point outside (point1, point2) segment
  double r = seg -(len1 + len2);
  if (len1>seg||len2>seg)
    r = seg - std::fabs(len1-len2);
  return r < 1e-8 && r > -1e-8;
}

// stream operators
template <class Type>
std::ostream& operator<<(std::ostream& s, vgl_infinite_line_3d<Type> const & p)
{
  return s << "<vgl_infinite_line_3d: origin " << p.x0() << " dir " << p.direction() << " >";
}

template <class Type>
std::istream& operator>>(std::istream& s, vgl_infinite_line_3d<Type>& p)
{
  s >> std::ws; // skip leading whitespace
  std::string temp;
  s >> temp >> temp; // skip "<vgl_infinite_line_3d: origin"
  s >> std::ws; // skip whitespace
  vgl_vector_2d<Type> x_0;
  s >> x_0;
  s >> std::ws; // skip whitespace
  s >> temp;// skip "dir "
  s >> std::ws; // skip whitespace
  vgl_vector_3d<Type> dir;
  s >> dir >> std::ws >> temp; // trailing " >"
  p.set(x_0, dir);
  return s;
}

#undef VGL_INFINITE_LINE_3D_INSTANTIATE
#define VGL_INFINITE_LINE_3D_INSTANTIATE(Type) \
template class vgl_infinite_line_3d<Type >;\
template std::istream& operator>>(std::istream&, vgl_infinite_line_3d<Type >&);\
template std::ostream& operator<<(std::ostream&, vgl_infinite_line_3d<Type > const&)

#endif // vgl_infinite_line_3d_hxx_
