//:
// \file
// \brief Functions to reflect shapes in various ways.
//  Renumbering necessary for symmetric shapes.  For instance,
//  if reflecting a face, the left eye becomes the right eye.
//  This would mangle a model, so we renumber.
// \author Tim Cootes

#include "msm_reflect_shape.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Reflect points in the line x=ax, then re-number
//  On exit, new_points[i] = points[relabel[i]] reflected in x=ax
void msm_reflect_shape_along_x(const msm_points& points,
                               const std::vector<unsigned>& sym_pts,
                               msm_points& new_points,
                               double ax)
{
  unsigned n= points.size();
  assert(sym_pts.size()==n);
  new_points.set_size(n);
  for (unsigned i=0;i<n;++i)
  {
    vgl_point_2d<double> p=points[i];
    double x = ax - (p.x()-ax);
    assert(sym_pts[i]<n);
    new_points.set_point(sym_pts[i],x,p.y());
  }
}
