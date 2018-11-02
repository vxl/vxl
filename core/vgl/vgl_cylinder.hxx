#ifndef vgl_cylinder_hxx_
#define vgl_cylinder_hxx_
//:
// \file

#include <iostream>
#include "vgl_cylinder.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
bool vgl_cylinder<T>::operator==(vgl_cylinder<T> const& cyl) const
{
  return (this==&cyl) ||
         ( cyl.center_ == this->center_ &&
           cyl.radius_ == this->radius_ &&
           cyl.length_ == this->length_ &&
           cyl.orient_ == this->orient_);
}

template <class T>
std::ostream& vgl_cylinder<T>::print(std::ostream& s) const
{
  return s << "<vgl_cylinder center=" << center_ << ','
           << " radius=" << radius_ << ", length =" << length_
           << ", direction=" << orient_ << '>';
}

template <class T>
std::ostream& operator<<(std::ostream& os, const vgl_cylinder<T>& cyl)
{
  return cyl.print(os);
}

template <class T>
std::istream& operator>>(std::istream& s, vgl_cylinder<T>& cyl)
{
  T center_x, center_y, center_z, radius, length, orient_x, orient_y, orient_z;
  s >> center_x >> center_y >> center_z >> radius >> length >> orient_x >> orient_y >> orient_z;
  cyl.set_center(vgl_point_3d<T>(center_x,center_y,center_z));
  cyl.set_radius(radius);
  cyl.set_length(length);
  cyl.set_orientation(vgl_vector_3d<T>(orient_x,orient_y,orient_z));
  return s;
}

#undef VGL_CYLINDER_INSTANTIATE
#define VGL_CYLINDER_INSTANTIATE(T) \
template class vgl_cylinder<T >; \
template std::ostream& operator<<(std::ostream&, vgl_cylinder<T >const&); \
template std::istream& operator>>(std::istream&, vgl_cylinder<T >&)

#endif // vgl_cylinder_hxx_
