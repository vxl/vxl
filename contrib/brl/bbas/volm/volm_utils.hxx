// This is contrib/brl/bbas/volm/volm_utils.hxx
#ifndef volm_utils_hxx_
#define volm_utils_hxx_
//:
// \file
#include <iostream>
#include <limits>
#include <cmath>
#include "volm_utils.h"
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define EPSILON 1E-6

// return true if the given point is strictly inside the box
template <class T>
static bool inside_box(vgl_box_2d<T> const& b, vgl_point_2d<T> const& pt)
{
  return ( pt.x() > b.min_x() && pt.x() < b.max_x() && pt.y() > b.min_y() && pt.y() < b.max_y() );
}

// check whether the given box is entirely inside the region defined by input point list
template <class T>
bool volm_utils::poly_contains(std::vector<vgl_point_2d<T> > const& sheet, vgl_box_2d<T> const& b)
{
  // check whether all 4 corners are inside
  vgl_polygon<T> poly(sheet);
  std::vector<vgl_point_2d<T> > corners;
  corners.push_back(b.min_point());
  corners.push_back(vgl_point_2d<T>(b.max_x(), b.min_y()));
  corners.push_back(b.max_point());
  corners.push_back(vgl_point_2d<T>(b.min_x(), b.max_y()));
  for (unsigned i = 0; i < corners.size(); i++)
    if (!poly.contains(corners[i]))
      return false;
  // all 4 corners are inside polygon, check if there is any of the boundary points is inside the box (not on the edges)
  for (unsigned i = 0; i < sheet.size(); i++)
    if (inside_box(b, sheet[i]))
      return false;
  return true;
}

template <class T>
vgl_polygon<T> volm_utils::poly_contains(std::vector<vgl_point_2d<T> > const& sheet, vgl_polygon<T> const& p_in)
{
  vgl_polygon<T> out_poly;
  out_poly.clear();
  vgl_polygon<T> p_out(sheet);
  unsigned n_sheet = p_in.num_sheets();
  for (unsigned i = 0; i < n_sheet; i++)  {
    unsigned n_pts = p_in[i].size();
    bool in = true;
    for (unsigned k = 0; (k < n_pts && in); k++)
      if (!p_out.contains(p_in[i][k]))
        in = false;
    if (in)
      out_poly.push_back(p_in[i]);
  }
  return out_poly;
}

template <class T>
bool volm_utils::poly_contains(vgl_polygon<T> const& poly, vgl_box_2d<T> const& b)
{
  return volm_utils::poly_contains(poly[0], b);
}

template <class T>
vgl_polygon<T> volm_utils::poly_contains(vgl_polygon<T> const& p_out, vgl_polygon<T> const& p_in)
{
  return volm_utils::poly_contains(p_out[0], p_in);
}

#undef VOLM_UTILS_INSTANTIATE
#define VOLM_UTILS_INSTANTIATE(T) \
template bool volm_utils::poly_contains(std::vector<vgl_point_2d<T > > const& sheet, vgl_box_2d<T > const& b); \
template bool volm_utils::poly_contains(vgl_polygon<T > const& poly, vgl_box_2d<T > const& b); \
template vgl_polygon<T> volm_utils::poly_contains(std::vector<vgl_point_2d<T > > const& p_out, vgl_polygon<T > const& p_in); \
template vgl_polygon<T> volm_utils::poly_contains(vgl_polygon<T > const& p_out, vgl_polygon<T > const& p_in)

#endif // volm_utils_hxx_
