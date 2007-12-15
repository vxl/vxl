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
// \endverbatim

#include <vcl_cmath.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

const inline double bmsh3d_dist_3d (const double& x1, const double& y1, const double& z1,
                                  const double& x2, const double& y2, const double& z2)
{
   return vcl_sqrt ( ((double) x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1) );
}

const inline double bmsh3d_sqdist_3d (const double& x1, const double& y1, const double& z1,
                                    const double& x2, const double& y2, const double& z2)
{
   return ((double) x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1);
}

const inline double bmsh3d_sqdist_3d (const vgl_point_3d<double>& pt1,
                                    const vgl_point_3d<double>& pt2)
{
   return bmsh3d_sqdist_3d (pt1.x(), pt1.y(), pt1.z(),
                          pt2.x(), pt2.y(), pt2.z());
}

vgl_box_3d<double> bmsh3d_reduce_box (const vgl_box_3d<double>& box, const double red);

vgl_box_3d<double> bmsh3d_extend_box (const vgl_box_3d<double>& box, const double ext);

vgl_box_3d<double> bmsh3d_extend_box_by_factor (const vgl_box_3d<double>& box, const float factor);

#endif
