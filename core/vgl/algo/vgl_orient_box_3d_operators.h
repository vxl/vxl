// This is core/vgl/algo/vgl_orient_box_3d_operators.h
#ifndef vgl_orient_box_3d_operators_h
#define vgl_orient_box_3d_operators_h
//:
// \file
// \brief Compute the minimal oriented bounding box for several 3D shapes
// \author Peter Vanroose
// \date   16 October 2009
//
// Given a 3D geometric object like a set of points, an ellipsoid, ...
// find the bounding box with the minimal volume that completely contains
// the given object. "Bounding box" is defined here as a Euclidean box
// (with mutually orthogonal rectangular faces), i.e., a vgl_orient_box_3d<T>.
//
// The class vgl_orient_box_3d_operators is actually a kind of namespace:
// it is just a collection of static methods, all returning a minimal box.
//
// \verbatim
//  Modifications
//   16-Oct-2009 Peter Vanroose - first (very minimal) version, only works for single-point input
// \endverbatim

#include <vector>
#include <vgl/algo/vgl_orient_box_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vgl_orient_box_3d_operators
{
  vgl_orient_box_3d_operators() = default; // The default constructor is private
 public:

  static vgl_orient_box_3d<T> minimal_box(vgl_point_3d<T> const& p) {
    vgl_box_3d<T> bb; bb.add(p); return bb;
  }

  static vgl_orient_box_3d<T> minimal_box(std::vector<vgl_point_3d<T> > const& plist);
};

#define VGL_ORIENT_BOX_3D_OPERATORS_INSTANTIATE(T) extern "Please #include <vgl/vgl_orient_box_3d_operators.hxx> instead"

#endif // vgl_orient_box_3d_operators_h
