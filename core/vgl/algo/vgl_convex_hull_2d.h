// This is core/vgl/algo/vgl_vgl_convex_hull_2d.h
#ifndef vgl_convex_hull_2d_h_
#define vgl_convex_hull_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Compute the convex hull of a 2-d point set
//
// \verbatim
//   July 13 2004 - J.L. Mundy - Converted to vgl interface
//   from code by Ken Clarkson (see .txx file for details)
// \endverbatim

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>

template <class T>
class vgl_convex_hull_2d
{
 public:
  vgl_convex_hull_2d (vcl_vector<vgl_point_2d<T> > const& points);
  ~vgl_convex_hull_2d (){};
  vgl_polygon<T>  hull();
 private:
  vgl_convex_hull_2d();
  bool hull_valid_;
  void compute_hull();
  vcl_vector<vgl_point_2d<T> > points_;
  vgl_polygon<T> hull_;
};

#define VGL_CONVEX_HULL_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_convex_hull_2d.txx first"
  
#endif

