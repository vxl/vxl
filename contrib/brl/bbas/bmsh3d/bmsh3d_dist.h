// This is brl/bbas/bmsh3d/bmsh3d_dist.h
#ifndef bmsh3d_dist_h_
#define bmsh3d_dist_h_
//:
// \file
// \brief Basic distance and squared distance computation.
//
// \author
//    Ming-Ching Chang Mar 07, 2007
//
// \verbatim
//  Modifications:
//   Peter Vanroose - 22 Feb 2009 - Added bmsh3d_dist_3d(point,point)
// \endverbatim

#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

inline double bmsh3d_sqdist_3d(const double& x1, const double& y1, const double& z1,
                               const double& x2, const double& y2, const double& z2)
{
  return (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
}

inline double bmsh3d_dist_3d(const double& x1, const double& y1, const double& z1,
                             const double& x2, const double& y2, const double& z2)
{
  return std::sqrt( bmsh3d_sqdist_3d(x1, y1, z1, x2, y2, z2) );
}

inline double bmsh3d_sqdist_3d(const vgl_point_3d<double>& pt1,
                               const vgl_point_3d<double>& pt2)
{
  return bmsh3d_sqdist_3d(pt1.x(), pt1.y(), pt1.z(),
                          pt2.x(), pt2.y(), pt2.z());
}

inline double bmsh3d_dist_3d(const vgl_point_3d<double>& pt1,
                             const vgl_point_3d<double>& pt2)
{
  return bmsh3d_dist_3d(pt1.x(), pt1.y(), pt1.z(),
                        pt2.x(), pt2.y(), pt2.z());
}

vgl_box_3d<double> bmsh3d_reduce_box (const vgl_box_3d<double>& box, const double red);

vgl_box_3d<double> bmsh3d_extend_box (const vgl_box_3d<double>& box, const double ext);

vgl_box_3d<double> bmsh3d_extend_box_by_factor (const vgl_box_3d<double>& box, const float factor);

#endif
