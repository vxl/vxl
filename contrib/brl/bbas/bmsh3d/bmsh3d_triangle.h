// This is brl/bbas/bmsh3d/bmsh3d_triangle.h
#ifndef bmsh3d_triangle_h_
#define bmsh3d_triangle_h_
//:
// \file
// \brief  Basic triangle geometric.
// \author Ming-Ching Chang (mcchang@lems.brown.edu)
// \date   May/31/2005
//
// \verbatim
//         Modify on Mar 07, 2007.
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_distance.h>
#include "bmsh3d_dist.h"
#include "bmsh3d_fuzzy_boolean.h"

//: return true for acute triangle or obtuse triangle.
//  the right triangle is treated as non-acute triangle.
inline bool bmsh3d_is_tri_non_acute(const vgl_point_3d<double>& v0,
                                    const vgl_point_3d<double>& v1,
                                    const vgl_point_3d<double>& v2)
{
  double dasq = bmsh3d_sqdist_3d (v0, v1);
  double dbsq = bmsh3d_sqdist_3d (v1, v2);
  double dcsq = bmsh3d_sqdist_3d (v0, v2);

  return bmsh3d_leq_m(dasq + dbsq, dcsq) ||
         bmsh3d_leq_m(dbsq + dcsq, dasq) ||
         bmsh3d_leq_m(dasq + dcsq, dbsq);
}

inline bool bmsh3d_footpt_on_line(const vgl_point_3d<double>& P,
                                  const vgl_point_3d<double>& A,
                                  const vgl_point_3d<double>& B)
{
  vgl_vector_3d<double> AP = P-A;
  vgl_vector_3d<double> AB = B-A;
  double l = vgl_distance (A, B);
  double t = dot_product (AP, AB) / l;

  return 0<=t && t<=l;
}

inline bool bmsh3d_footpt_on_line(const vgl_point_3d<double>& P,
                                  const vgl_point_3d<double>& A,
                                  const vgl_point_3d<double>& B,
                                  double& t, double& l)
{
  vgl_vector_3d<double> AP = P-A;
  vgl_vector_3d<double> AB = B-A;
  l = vgl_distance (A, B);
  t = dot_product (AP, AB) / l;

  return 0<=t && t<=l;
}

#endif // bmsh3d_triangle_h_
