// This is oxl/mvl/PMatrixComputeLinear.h
#ifndef PMatrixComputeLinear_h_
#define PMatrixComputeLinear_h_
//:
// \file
//
// PMatrixComputeLinear contains a linear method to calculate the P matrix
// from at least six 3D-to-2D point correspondences.  Precisely, given
// 3D points $\vec X_i$ and their images $\vec u_i$, we compute $P$ such that
// $P \vec X_i \sim \vec u_i$.
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim

#include <mvl/PMatrixCompute.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
class PMatrix;

class PMatrixComputeLinear : public PMatrixCompute
{
 public:
  bool compute(std::vector<HomgPoint2D> const& points1, std::vector<HomgPoint3D> const& points2, PMatrix *P);
  bool compute(std::vector<vgl_homg_point_2d<double> > const& points1,
               std::vector<vgl_homg_point_3d<double> > const& points2,
               PMatrix *P);
};

#endif // PMatrixComputeLinear_h_
