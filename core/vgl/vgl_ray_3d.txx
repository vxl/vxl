// This is core/vgl/vgl_ray_3d.txx
#ifndef vgl_ray_3d_txx_
#define vgl_ray_3d_txx_

#include "vgl_ray_3d.h"
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for fabs
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_tolerance.h>
template <class Type>
bool vgl_ray_3d<Type>::contains(const vgl_point_3d<Type>& p ) const
{
  vgl_point_3d<Type> pcls = vgl_closest_point(*this, p); 
  Type len = static_cast<Type>(length(pcls-p));
  if(len*len > static_cast<Type>(10) * vcl_max(vgl_tolerance<Type>::position, p.x()*p.x()+p.y()*p.y()+p.z()*p.z()) * vgl_tolerance<Type>::position)
    return false;
  Type dp = dot_product(t_, pcls-p0_);
  Type tol = vgl_tolerance<Type>::position;
  return  dp >= -tol;
}


// stream operators
template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_ray_3d<Type> const & p)
{
  return s << "<vgl_ray_3d: origin" << p.origin() << " dir " << p.direction() << " >";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_ray_3d<Type>& r)
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
template vcl_istream& operator>>(vcl_istream&, vgl_ray_3d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_ray_3d<Type > const&)

#endif // vgl_ray_3d_txx_
