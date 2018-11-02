// This is oxl/mvl/PMatrixComputeLinear.cxx
#include <iostream>
#include <vector>
#include "PMatrixComputeLinear.h"
//:
//  \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>

#include <mvl/PMatrix.h>

//-----------------------------------------------------------------------------
//
//: Compute a projection matrix using linear least squares.
// Input is a list of 3D-2D point correspondences.
//
// Return false if the calculation fails or there are fewer than six point
// matches in the list.
//
// The algorithm finds the nullvector of the following design matrix, where
// the 3d points $\vec X^i$ are projected into 2d points $\vec u^i$.
// \f[
// \left(
// \begin{array}{ccc}
// \vec X^1 u^1_3        & 0_{4}             & -\vec X^1 u^1_1 \cr
// 0_4                   & \vec X^1 u^1_3    & -\vec X^1 u^1_2 \cr
// \vdots                & \vdots & \vdots \cr
// \vec X^n u^n_3        & 0_{4}             & -\vec X^n u^n_1 \cr
// 0_4                   & \vec X^n u^n_3    & -\vec X^n u^n_2 \cr
// \end{array}
// \right)
// \f]
bool
PMatrixComputeLinear::compute(std::vector<vgl_homg_point_2d<double> > const& points1,
                              std::vector<vgl_homg_point_3d<double> > const& points2,
                              PMatrix *P)
{
  assert(P);
  assert(points1.size() >= 6);
  assert(points2.size() == points1.size());

  unsigned npts = points1.size();
  vnl_matrix<double> a_matrix(npts * 2, 12);

  for (unsigned i = 0; i < npts; i++) {
    vgl_homg_point_2d<double> const& u = points1[i];
    vgl_homg_point_3d<double> const& X = points2[i];

    unsigned row_index = i * 2;
    a_matrix(row_index,  0) =  X.x() * u.w();
    a_matrix(row_index,  1) =  X.y() * u.w();
    a_matrix(row_index,  2) =  X.z() * u.w();
    a_matrix(row_index,  3) =  X.w() * u.w();
    a_matrix(row_index,  4) =  0;
    a_matrix(row_index,  5) =  0;
    a_matrix(row_index,  6) =  0;
    a_matrix(row_index,  7) =  0;
    a_matrix(row_index,  8) = -X.x() * u.x();
    a_matrix(row_index,  9) = -X.y() * u.x();
    a_matrix(row_index, 10) = -X.z() * u.x();
    a_matrix(row_index, 11) = -X.w() * u.x();

    row_index = i * 2 + 1;
    a_matrix(row_index,  0) =  0;
    a_matrix(row_index,  1) =  0;
    a_matrix(row_index,  2) =  0;
    a_matrix(row_index,  3) =  0;
    a_matrix(row_index,  4) =  X.x() * u.w();
    a_matrix(row_index,  5) =  X.y() * u.w();
    a_matrix(row_index,  6) =  X.z() * u.w();
    a_matrix(row_index,  7) =  X.w() * u.w();
    a_matrix(row_index,  8) = -X.x() * u.y();
    a_matrix(row_index,  9) = -X.y() * u.y();
    a_matrix(row_index, 10) = -X.z() * u.y();
    a_matrix(row_index, 11) = -X.w() * u.y();
  }

  a_matrix.normalize_rows();
  vnl_svd<double> svd(a_matrix);

  P->set(svd.nullvector().data_block());

  return true;
}

bool
PMatrixComputeLinear::compute (std::vector<HomgPoint2D> const& points1, std::vector<HomgPoint3D> const& points2, PMatrix *P)
{
  assert(P);
  assert(points1.size() >= 6);
  assert(points2.size() == points1.size());

  unsigned npts = points1.size();
  vnl_matrix<double> a_matrix(npts * 2, 12);

  for (unsigned i = 0; i < npts; i++) {
    HomgPoint2D const& u = points1[i];
    HomgPoint3D const& X = points2[i];

    unsigned row_index = i * 2;
    a_matrix(row_index,  0) =  X.x() * u.w();
    a_matrix(row_index,  1) =  X.y() * u.w();
    a_matrix(row_index,  2) =  X.z() * u.w();
    a_matrix(row_index,  3) =  X.w() * u.w();
    a_matrix(row_index,  4) =  0;
    a_matrix(row_index,  5) =  0;
    a_matrix(row_index,  6) =  0;
    a_matrix(row_index,  7) =  0;
    a_matrix(row_index,  8) = -X.x() * u.x();
    a_matrix(row_index,  9) = -X.y() * u.x();
    a_matrix(row_index, 10) = -X.z() * u.x();
    a_matrix(row_index, 11) = -X.w() * u.x();

    row_index = i * 2 + 1;
    a_matrix(row_index,  0) =  0;
    a_matrix(row_index,  1) =  0;
    a_matrix(row_index,  2) =  0;
    a_matrix(row_index,  3) =  0;
    a_matrix(row_index,  4) =  X.x() * u.w();
    a_matrix(row_index,  5) =  X.y() * u.w();
    a_matrix(row_index,  6) =  X.z() * u.w();
    a_matrix(row_index,  7) =  X.w() * u.w();
    a_matrix(row_index,  8) = -X.x() * u.y();
    a_matrix(row_index,  9) = -X.y() * u.y();
    a_matrix(row_index, 10) = -X.z() * u.y();
    a_matrix(row_index, 11) = -X.w() * u.y();
  }

  a_matrix.normalize_rows();
  vnl_svd<double> svd(a_matrix);

  P->set(svd.nullvector().data_block());

  return true;
}
