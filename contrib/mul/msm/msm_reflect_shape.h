#ifndef msm_reflect_shape_h_
#define msm_reflect_shape_h_
//:
// \file
// \brief Set of 2D points, stored in a vnl_vector (x0,y0,x1,y1...)
// \author Tim Cootes

#include <msm/msm_points.h>

//: Reflect points in the line x=ax, then re-number
//  On exit, new_points[i] = points[sym_pts[i]] reflected in x=ax
//  \param sym_pts[i] defines the point symmetric with point i
//  Renumbering necessary for symmetric shapes.  For instance,
//  if reflecting a face, the left eye becomes the right eye.
//  This would mangle a model, so we renumber.
void msm_reflect_shape_along_x(const msm_points& points,
                               const std::vector<unsigned>& sym_pts,
                               msm_points& new_points,
                               double ax=0.0);

#endif // msm_reflect_shape_h_
