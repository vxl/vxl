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
vgl_point_3d<Type>::vgl_point_3d(vgl_plane_3d<Type> const& pl1,
                                 vgl_plane_3d<Type> const& pl2,
                                 vgl_plane_3d<Type> const& pl3)
{
  vgl_homg_plane_3d<Type> h1(pl1.nx(), pl1.ny(), pl1.nz(), pl1.d());
  vgl_homg_plane_3d<Type> h2(pl2.nx(), pl2.ny(), pl2.nz(), pl2.d());
  vgl_homg_plane_3d<Type> h3(pl3.nx(), pl3.ny(), pl3.nz(), pl3.d());
  vgl_homg_point_3d<Type> p(h1, h2, h3); // do homogeneous intersection
  set(p.x()/p.w(), p.y()/p.w(), p.z()/p.w()); // could be infinite!
}

//: Write "<vgl_point_3d x,y,z> " to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_point_3d<Type> const& p) {
  return s << "<vgl_point_3d "<< p.x() << "," << p.y() << "," << p.z() << "> ";
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
