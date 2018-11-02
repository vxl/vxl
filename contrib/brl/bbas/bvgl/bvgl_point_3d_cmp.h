// This is brl/bbas/bvgl/bvgl_point_3d_cmp.h
#ifndef bvgl_point_3d_cmp_h
#define bvgl_point_3d_cmp_h
//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs).
// \author Isabel Restrepo mir@lems.brown.edu
// \date  27-Jul-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <functional>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A comparison functor for vgl_point_3d's. Needed to create a std::set of vgl_point_3d<int>'s.
template <class T>
class bvgl_point_3d_cmp
{
 public:
  bvgl_point_3d_cmp() = default;

  bool operator()(vgl_point_3d<T> const& v0, vgl_point_3d<T> const& v1) const
  {
    if (v0.z() != v1.z())
      return v0.z() < v1.z();
    else if (v0.y() != v1.y())
      return v0.y() < v1.y();
    else
      return v0.x() < v1.x();
  }
};

#endif
