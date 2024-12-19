#ifndef vgl_cylinder_3d_hxx_
#define vgl_cylinder_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_cylinder_3d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
bool
vgl_cylinder_3d<T>::operator==(const vgl_cylinder_3d<T> & cyl) const
{
  return (this == &cyl) || (cyl.center_ == this->center_ && cyl.radius_ == this->radius_ &&
                            cyl.length_ == this->length_ && cyl.orient_ == this->orient_);
}

template <class T>
std::ostream &
vgl_cylinder_3d<T>::print(std::ostream & s) const
{
  return s << "<vgl_cylinder_3d center=" << center_ << ',' << " radius=" << radius_ << ", length =" << length_
           << ", direction=" << orient_ << '>';
}

template <class T>
std::ostream &
operator<<(std::ostream & os, const vgl_cylinder_3d<T> & cyl)
{
  return cyl.print(os);
}

template <class T>
std::istream &
operator>>(std::istream & s, vgl_cylinder_3d<T> & cyl)
{
  T center_x, center_y, center_z, radius, length, orient_x, orient_y, orient_z;
  s >> center_x >> center_y >> center_z >> radius >> length >> orient_x >> orient_y >> orient_z;
  cyl.set_center(vgl_point_3d<T>(center_x, center_y, center_z));
  cyl.set_radius(radius);
  cyl.set_length(length);
  cyl.set_orientation(vgl_vector_3d<T>(orient_x, orient_y, orient_z));
  return s;
}

#undef VGL_CYLINDER_3D_INSTANTIATE
#define VGL_CYLINDER_3D_INSTANTIATE(T)                                            \
  template class vgl_cylinder_3d<T>;                                              \
  template std::ostream & operator<<(std::ostream &, vgl_cylinder_3d<T> const &); \
  template std::istream & operator>>(std::istream &, vgl_cylinder_3d<T> &)

#endif // vgl_cylinder_3d_hxx_
