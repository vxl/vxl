// This is brl/bbas/bvgl/bvgl_triangle_3d.hxx
#ifndef bvgl_triangle_3d_hxx_
#define bvgl_triangle_3d_hxx_
//:
// \file

#include <iostream>
#include <algorithm>
#include <cmath>
#include "bvgl_triangle_3d.h"
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

// Constructors/Destructor---------------------------------------------------

template <class Type>
bvgl_triangle_3d<Type>::bvgl_triangle_3d()
{
  points_[0].set(0,0,0);
  points_[1].set(0,0,0);
  points_[2].set(0,0,0);
}

template <class Type>
bvgl_triangle_3d<Type>::bvgl_triangle_3d(vgl_point_3d<Type> const& corner1,
                                         vgl_point_3d<Type> const& corner2,
                                         vgl_point_3d<Type> const& corner3)
{
  points_[0] = corner1;
  points_[1] = corner2;
  points_[2] = corner3;
}

template <class Type>
std::ostream& bvgl_triangle_3d<Type>::print(std::ostream& s) const
{
  return s << "<bvgl_triangle_3d "<< ' ' << points_[0] << ' '
                                         << points_[1] << ' '
                                         << points_[2] << '>';
}

template <class Type>
std::ostream& bvgl_triangle_3d<Type>::write(std::ostream& s) const
{
  return s << points_[0] << ' ' << points_[1] << ' ' << points_[2] << '>';
}

template <class Type>
std::istream& bvgl_triangle_3d<Type>::read(std::istream& is)
{
  return is >> points_[0] >> points_[1] >> points_[2];
}

//: Write box to stream
template <class Type>
std::ostream&  operator<<(std::ostream& s, bvgl_triangle_3d<Type> const& p)
{
  return p.print(s);
}

//: Read box from stream
template <class Type>
std::istream&  operator>>(std::istream& is,  bvgl_triangle_3d<Type>& p)
{
  return p.read(is);
}

#undef BVGL_TRIANGLE_3D_INSTANTIATE
#define BVGL_TRIANGLE_3D_INSTANTIATE(Type) \
template class bvgl_triangle_3d<Type >;\
template std::ostream& operator<<(std::ostream&, bvgl_triangle_3d<Type > const& p);\
template std::istream& operator>>(std::istream&, bvgl_triangle_3d<Type >& p)

#endif // bvgl_triangle_3d_hxx_
