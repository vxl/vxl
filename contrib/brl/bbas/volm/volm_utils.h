// This is contrib/brl/bbas/volm/volm_utils.h
#ifndef volm_utils_h_
#define volm_utils_h_
//:
// \file
// \brief Set of utility functions for Volumetric Matching and 3-d indexing library
// \author Yi Dong
// \date Oct 16, 2014
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
#include <iostream>
#include <string>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_intersection.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class volm_utils
{
public:

  //: return true if box is entirely inside the region defined by input point list
  template <class T>
  static bool poly_contains(vgl_polygon<T> const& poly, vgl_box_2d<T> const& b);
  template <class T>
  static bool poly_contains(std::vector<vgl_point_2d<T> > const& sheet, vgl_box_2d<T> const& b);

  //: return polygon sheet that lie entirely inside the polygon p_out
  template <class T>
  static vgl_polygon<T> poly_contains(vgl_polygon<T> const& p_out, vgl_polygon<T> const& p_in);
  template <class T>
  static vgl_polygon<T> poly_contains(std::vector<vgl_point_2d<T> > const& p_out, vgl_polygon<T> const& p_in);

  static std::string volm_src_root();

private:  // only static function in this class: disallow instantiation
  volm_utils() = delete;
};

#define VOLM_UTILS_INSTANTIATE(T) extern "Please #include <volm/volm_utils.hxx>"

#endif // volm_utils_h_
