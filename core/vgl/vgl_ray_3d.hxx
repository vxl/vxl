// This is core/vgl/vgl_ray_3d.hxx
#ifndef vgl_ray_3d_hxx_
#define vgl_ray_3d_hxx_

#include <algorithm>
#include <iostream>
#include <cmath>
#include "vgl_ray_3d.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_tolerance.h>
template <class Type>
bool vgl_ray_3d<Type>::contains(const vgl_point_3d<Type>& p ) const
{
  vgl_point_3d<Type> pcls = vgl_closest_point(*this, p);
  Type len = static_cast<Type>(length(pcls-p));
  if(len*len > static_cast<Type>(10) * std::max(vgl_tolerance<Type>::position, p.x()*p.x()+p.y()*p.y()+p.z()*p.z()) * vgl_tolerance<Type>::position)
    return false;
  Type dp = dot_product(t_, pcls-p0_);
  Type tol = vgl_tolerance<Type>::position;
  return  dp >= -tol;
}


// stream operators
template <class Type>
std::ostream& operator<<(std::ostream& s, vgl_ray_3d<Type> const & p)
{
  return s << "<vgl_ray_3d: origin" << p.origin() << " dir " << p.direction() << " >";
}

template <class Type>
std::istream& operator>>(std::istream& s, vgl_ray_3d<Type>& r)
{
  vgl_point_3d<Type> p0;
  vgl_vector_3d<Type> dir;
  s >> p0 >> dir;
  r.set(p0, dir);
  return s;
}

#undef VGL_RAY_3D_INSTANTIATE
#define VGL_RAY_3D_INSTANTIATE(Type) \
template class vgl_ray_3d<Type >;\
template std::istream& operator>>(std::istream&, vgl_ray_3d<Type >&);\
template std::ostream& operator<<(std::ostream&, vgl_ray_3d<Type > const&)

#endif // vgl_ray_3d_hxx_
