// This is core/vgl/algo/vgl_ellipsoid_3d.txx
#ifndef vgl_ellipsoid_3d_txx_
#define vgl_ellipsoid_3d_txx_
//:
// \file

#include "vgl_ellipsoid_3d.h"
#include <vcl_iostream.h>

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
vcl_ostream& vgl_ellipsoid_3d<T>::print(vcl_ostream& s) const
{
  return s << "<vgl_ellipsoid_3d center=" << center_
           << ", orientation=" << orientation_ << ", size=("
           << x_halflength_ << ',' << y_halflength_ << ',' << z_halflength_ << ")>";
}

#undef VGL_ELLIPSOID_3D_INSTANTIATE
#define VGL_ELLIPSOID_3D_INSTANTIATE(T) \
template class vgl_ellipsoid_3d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_ellipsoid_3d<T >const&)

#endif // vgl_ellipsoid_3d_txx_
