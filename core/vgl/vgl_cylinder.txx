#ifndef vgl_cylinder_txx_
#define vgl_cylinder_txx_
//:
// \file

#include "vgl_cylinder.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>

template <class Type>
bool vgl_cylinder<Type>::contains(vgl_point_3d<Type> point)
{
  return false;
}
template <class T>
bool vgl_cylinder<T>::operator==(vgl_cylinder<T> const& cyl) const
{
  return (this==&cyl) ||
         ( cyl.center_ == this->center_
         && cyl.radius_ ==  this->radius_
         && cyl.length_ == this->length_
         && cyl.orient_ == this->orient_);
}

template <class T>
vcl_ostream& vgl_cylinder<T>::print(vcl_ostream& s) const
{
  
   return s<< "<vgl_cylinder "
            << "center=" << center_ << "," 
            << " radius=" << radius_ << ", lenght =" << length_ 
            << ", direction=" << orient_ 
            << ">";
}

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, const vgl_cylinder<T>& cyl)
{
  return cyl.print(os);
}

template <class T>
vcl_istream& operator>>(vcl_istream& s, vgl_cylinder<T>& cyl)
{
  T center_x, center_y, center_z, radius, length, orient_x, orient_y, orient_z;
  s >> center_x >> center_y >> center_z >> radius >> length >> orient_x >> orient_y >> orient_z;
  cyl.set_center(vgl_point_3d<double> (center_x, center_y, center_z));
  cyl.set_radius(radius);
  cyl.set_length(length);
  cyl.set_orientation(vgl_vector_3d<double> (orient_x, orient_y, orient_z));
  return s;
}

#undef VGL_CYLINDER_INSTANTIATE
#define VGL_CYLINDER_INSTANTIATE(T) \
template class vgl_cylinder<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_cylinder<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_cylinder<T >&)

#endif