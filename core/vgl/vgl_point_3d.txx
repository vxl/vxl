// This is vxl/vgl/vgl_point_3d.txx
#ifndef vgl_point_3d_txx_
#define vgl_point_3d_txx_

#include "vgl_point_3d.h"

#include <vcl_cmath.h>

template <class Type>
bool vgl_point_3d<Type>::operator==(const vgl_point_3d<Type> &other) const
{
  return (this==&other) ||
         (   (this->x()==other.x()) && (this->y()==other.y()) && (this->z()==other.z()));
}

template <class Type>
vgl_point_3d<Type> vgl_point_3d<Type>::operator+(const vgl_point_3d<Type>& that) const
{
   vgl_point_3d<Type> result((this->x()+that.x()),this->y()+that.y(),
    this->z()+that.z());
   return result;
}

template <class Type>
vgl_point_3d<Type> vgl_point_3d<Type>::operator-(const vgl_point_3d<Type>& that) const
{
   vgl_point_3d<Type> result((this->x()-that.x()),this->y()-that.y(),this->z()-that.z());
   return result;
}

#define VGL_POINT_3D_INSTANTIATE(T) \
template class vgl_point_3d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vgl_point_3d<T >&); \
template vcl_istream& operator>>(vcl_istream&, vgl_point_3d<T >&)

#endif // vgl_point_3d_txx_
