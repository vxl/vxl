#ifndef msm_equally_space_h_
#define msm_equally_space_h_
//:
// \file
// \brief Functions to equally space points along smooth curves
// \author Tim Cootes

#include <msm/msm_points.h>
#include <msm/msm_curve.h>

//: Slide points along smooth curve through current points to equally space them.
// If curve is open, the first and last points are fixed.
// If curve is closed, the first point is fixed.
// All points not on curve are left unchanged
// \param min_bez_sep Smallest separation between points on bezier curve used for smoothing
void msm_equally_space(msm_points& points,
                       msm_curve& curve,
                       double min_bez_sep=1.0);

//: Slide points along each curve through current points to equally space them.
// If curve is open, the first and last points are fixed.
// If curve is closed, the first point is fixed.
// All points not on curves are left unchanged
// \param min_bez_sep Smallest separation between points on bezier curve used for smoothing
void msm_equally_space(msm_points& points,
                       msm_curves& curves,
                       double min_bez_sep=1.0);

#endif // msm_equally_space_h_
