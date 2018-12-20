#ifndef msm_rotate_to_axes_h_
#define msm_rotate_to_axes_h_
//:
// \file
// \brief Function to rotate a shape to align with the axes.
// \author Tim Cootes

#include <msm/msm_points.h>

//: Translate shape to origin then rotate so main axis is along angle A (radians)
// If ref_pt>=0 then ensures that points[ref_pt] is on the positive size
// when measured along direction A.  This enables disambiguation (main axis
// of points may be arbitrarily flipped by 180 degrees).
void msm_rotate_to_axes(msm_points& points, double A=0, int ref_pt=-1);

#endif
