// This is vxl/vgl/vgl_point_2d.txx
#ifndef vgl_point_2d_txx_
#define vgl_point_2d_txx_

#include "vgl_point_2d.h"
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>

template <class Type>
vgl_point_2d<Type>::vgl_point_2d(vgl_line_2d<Type> const& l1,
                                 vgl_line_2d<Type> const& l2)
{
  vgl_homg_line_2d<Type> h1(l1.a(), l1.b(), l1.c());
  vgl_homg_line_2d<Type> h2(l2.a(), l2.b(), l2.c());
  vgl_homg_point_2d<Type> p(h1, h2); // do homogeneous intersection
  set(p.x()/p.w(), p.y()/p.w()); // could be infinite!
}

template <class Type>
bool vgl_point_2d<Type>::operator==(const vgl_point_2d<Type> &other) const
{
  return (this==&other) ||
         (   (this->x()==other.x()) && (this->y()==other.y()));
}

template <class Type>
vgl_point_2d<Type> vgl_point_2d<Type>::operator+(const vgl_point_2d<Type>& that) const
{
   vgl_point_2d<Type> result((this->x()+that.x()),this->y()+that.y());
   return result;
}

template <class Type>
vgl_point_2d<Type> vgl_point_2d<Type>::operator-(const vgl_point_2d<Type>& that) const
{
   vgl_point_2d<Type> result((this->x()-that.x()),this->y()-that.y());
   return result;
}

template <class Type>
vgl_point_2d<Type> vgl_point_2d<Type>::operator*(double d) const
{
   vgl_point_2d<Type> result(Type(this->x()*d),Type(this->y()*d));
   return result;
}

template <class Type>
vgl_point_2d<Type> vgl_point_2d<Type>::operator/(double d) const
{
   vgl_point_2d<Type> result(Type(this->x()/d),Type(this->y()/d));
   return result;
}

template <class Type>
vgl_point_2d<Type>& vgl_point_2d<Type>::operator+=(const vgl_point_2d<Type>& that)
{
   data_[0] += that.data_[0];
   data_[1] += that.data_[1];
   return *this;
}

template <class Type>
vgl_point_2d<Type>& vgl_point_2d<Type>::operator-=(const vgl_point_2d<Type>& that)
{
   data_[0] -= that.data_[0];
   data_[1] -= that.data_[1];
   return *this;
}
template <class Type>
vgl_point_2d<Type>& vgl_point_2d<Type>::operator/=(double d)
{
   data_[0] /= d;
   data_[1] /= d;
   return *this;
}

template <class Type>
vgl_point_2d<Type>& vgl_point_2d<Type>::operator*=(double d)
{
   data_[0] *= d;
   data_[1] *= d;
   return *this;
}

#define VGL_POINT_2D_INSTANTIATE(T) \
template class vgl_point_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vgl_point_2d<T >&); \
template vcl_istream& operator>>(vcl_istream&, vgl_point_2d<T >&)

#endif // vgl_point_2d_txx_
