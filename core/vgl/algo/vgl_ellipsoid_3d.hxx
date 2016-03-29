// This is core/vgl/algo/vgl_ellipsoid_3d.hxx
#ifndef vgl_ellipsoid_3d_hxx_
#define vgl_ellipsoid_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_ellipsoid_3d.h"
#include <vcl_compiler.h>

template <class T>
bool vgl_ellipsoid_3d<T>::operator==(vgl_ellipsoid_3d<T> const& e) const
{
  return (this==&e) ||
         ( e.center() == this->center_ &&
           e.x_halflength() == this->x_halflength_ &&
           e.y_halflength() == this->y_halflength_ &&
           e.z_halflength() == this->z_halflength_ &&
           e.orientation()  == this->orientation_);
}

template <class T>
std::ostream& vgl_ellipsoid_3d<T>::print(std::ostream& s) const
{
  return s << "<vgl_ellipsoid_3d center=" << center_
           << ", orientation=" << orientation_ << ", size=("
           << x_halflength_ << ',' << y_halflength_ << ',' << z_halflength_ << ")>";
}

#undef VGL_ELLIPSOID_3D_INSTANTIATE
#define VGL_ELLIPSOID_3D_INSTANTIATE(T) \
template class vgl_ellipsoid_3d<T >; \
template std::ostream& operator<<(std::ostream&, vgl_ellipsoid_3d<T >const&)

#endif // vgl_ellipsoid_3d_hxx_
