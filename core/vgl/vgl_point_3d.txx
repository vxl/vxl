// This is vxl/vgl/vgl_point_3d.txx
#ifndef vgl_point_3d_txx_
#define vgl_point_3d_txx_

#include "vgl_point_3d.h"
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

#include <vcl_iostream.h>
#include <vcl_cmath.h>

//: Construct from homogeneous point
template <class Type>
vgl_point_3d<Type>::vgl_point_3d(vgl_homg_point_3d<Type> const& p)
  : x_(p.x()/p.w()), y_(p.y()/p.w()), z_(p.z()/p.w()) // could be infinite!
{
}

//: Construct from 3 planes (intersection).
template <class Type>
vgl_point_3d<Type>::vgl_point_3d(vgl_plane_3d<Type> const& l1,
                                 vgl_plane_3d<Type> const& l2,
                                 vgl_plane_3d<Type> const& l3)
{
  vgl_homg_plane_3d<Type> h1(l1.nx(), l1.ny(), l1.nz(), l1.d());
  vgl_homg_plane_3d<Type> h2(l2.nx(), l2.ny(), l2.nz(), l2.d());
  vgl_homg_plane_3d<Type> h3(l3.nx(), l3.ny(), l3.nz(), l3.d());
  vgl_homg_point_3d<Type> p(h1, h2, h3); // do homogeneous intersection
  set(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()); // could be infinite!
}

//: Write "x y z " to stream
template <class Type>
vcl_ostream& vgl_point_3d<Type>::write(vcl_ostream& s) const {
  return s << x() << " " << y() << " " << z() << " ";
}

//: Write "<vgl_point_3d x,y,z> " to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_point_3d<Type>& p) {
  return s << "<vgl_point_3d "
           << p.x() << "," << p.y() << "," << p.z() << "> ";
}

//: Read x y z from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_point_3d<Type>& p) {
  Type x, y, z; is >> x >> y >> z; p.set(x,y,z); return is;
}

#define VGL_POINT_3D_INSTANTIATE(T) \
template class vgl_point_3d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vgl_point_3d<T >&); \
template vcl_istream& operator>>(vcl_istream&, vgl_point_3d<T >&)

#endif // vgl_point_3d_txx_
