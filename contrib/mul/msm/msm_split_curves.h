#ifndef msm_split_curves_h_
#define msm_split_curves_h_

//:
// \file
// \brief Functions to split curves at particular points
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include <msm/msm_curve.h>
#include <vgl/vgl_point_2d.h>

//: Sets is_junc[i] to true if any point mentioned in >1 curve
void msm_get_junctions(const msm_curves& curves,
                       unsigned n_pts,
                       std::vector<bool>& is_junc);

//: Splits curve at internal point p with end_pt[p] true
// Appends new curves to new_curves.
// For example, if end_pt[3] is true, and one open curve is 1-2-3-4-5,
// then it will be split into two curves 1-2-3 and 3-4-5.
// If the curve was closed, it would be turned into the open curve 3-4-5-1-2-3
void msm_split_curve(const msm_curve& curve,
                      const std::vector<bool>& end_pt,
                      msm_curves& new_curves);

//: Splits any curve at internal points p where end_pt[p] is true
// For example, if end_pt[3] is true, and one open curve is 1-2-3-4-5,
// then it will be split into two curves 1-2-3 and 3-4-5.
// If the curve was closed, it would be turned into the open curve 3-4-5-1-2-3
void msm_split_curves(const msm_curves& curves,
                      const std::vector<bool>& end_pt,
                      msm_curves& new_curves);

//: Set corner[p]==true if p is on curves and arcs to p have cos<cos_thresh
//  Consider three points along the curve, p1-p2-p3.  Let a be the unit vector
//  along p2-p1 and b be the unit vector along p2-p3.  Thus a.b gives the cosine
//  of the angle at p2.  If a.b>cos_thresh this is considered a corner.
void msm_find_corners(const msm_curves& curves,
                      const std::vector<vgl_point_2d<double> >& pts,
                      double cos_thresh,
                      std::vector<bool>& corner);

//: Splits any curve at junctions or where curvature is high.
//  Junctions are internal points belonging to more than one curve.
//  Curvature at a point is measured by the angle between two arcs going
//  from that point to its neighbours.  High curvature points are those with
//  cos(angle)>cos_thresh.
void msm_split_curves(const msm_curves& curves,
                      const std::vector<vgl_point_2d<double> >& pts,
                      double cos_thresh,
                      msm_curves& new_curves);


#endif // msm_split_curves_h_
