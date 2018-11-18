// This is core/vgl/algo/vgl_h_matrix_3d_compute_linear.cxx
#include <iostream>
#include <cmath>
#include "vgl_h_matrix_3d_compute_linear.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/algo/vnl_svd.h>

constexpr int TM_UNKNOWNS_COUNT = 16;
constexpr double DEGENERACY_THRESHOLD = 0.00001;


//: Compute a 3D-to-3D homography using linear least squares.
// Returns false if the calculation fails or there are fewer than five point
// matches in the list.  The algorithm finds the nullvector of the $6 n \times 16$ design
// matrix:

//:Assumes all corresponding points have equal weight
bool vgl_h_matrix_3d_compute_linear::
solve_linear_problem(std::vector<vgl_homg_point_3d<double> > const& p1,
                     std::vector<vgl_homg_point_3d<double> > const& p2,
                     vgl_h_matrix_3d<double>& H)
{
  int n = p1.size();
  //transform the point sets and fill the design matrix
  vnl_matrix<double> D(n*6, TM_UNKNOWNS_COUNT);

  int row = 0;
  for (int i = 0; i < n; i++) {
    //double x1 = p1[i].x(), x2 = p1[i].y(), x3 = p1[i].z(), x4 = p1[i].w();
    //double y1 = p2[i].x(), y2 = p2[i].y(), y3 = p2[i].z(), y4 = p2[i].w();
    double x1 = p2[i].x(), x2 = p2[i].y(), x3 = p2[i].z(), x4 = p2[i].w();
    double y1 = p1[i].x(), y2 = p1[i].y(), y3 = p1[i].z(), y4 = p1[i].w();

    D(row, 0) = -x2*y1;   D(row, 1) = x1*y1;   D(row, 2) = 0;      D(row, 3) = 0;
    D(row, 4) = -x2*y2;   D(row, 5) = x1*y2;   D(row, 6) = 0;      D(row, 7) = 0;
    D(row, 8) = -x2*y3;   D(row, 9) = x1*y3;   D(row, 10) = 0;     D(row, 11) = 0;
    D(row, 12) = -x2*y4;  D(row, 13) = x1*y4;  D(row, 14) = 0;     D(row, 15) = 0;
    ++row;
    D(row, 0) = -x3*y1;   D(row, 1) = 0;       D(row, 2) = x1*y1;  D(row, 3) = 0;
    D(row, 4) = -x3*y2;   D(row, 5) = 0;       D(row, 6) = x1*y2;  D(row, 7) = 0;
    D(row, 8) = -x3*y3;   D(row, 9) = 0;       D(row, 10) = x1*y3; D(row, 11) = 0;
    D(row, 12) = -x3*y4;  D(row, 13) = 0;      D(row, 14) = x1*y4;   D(row, 15) = 0;
    ++row;
    D(row, 0) = -x4*y1;   D(row, 1) = 0;       D(row, 2) = 0;      D(row, 3) = x1*y1;
    D(row, 4) = -x4*y2;   D(row, 5) = 0;       D(row, 6) = 0;      D(row, 7) = x1*y2;
    D(row, 8) = -x4*y3;   D(row, 9) = 0;       D(row, 10) = 0;     D(row, 11) = x1*y3;
    D(row, 12) = -x4*y4;  D(row, 13) = 0;      D(row, 14) = 0;     D(row, 15) = x1*y4;
    ++row;
    D(row, 0) = 0;        D(row, 1) = -x4*y1;  D(row, 2) = 0;      D(row, 3) = x2*y1;
    D(row, 4) = 0;        D(row, 5) = -x4*y2;  D(row, 6) = 0;      D(row, 7) = x2*y2;
    D(row, 8) = 0;        D(row, 9) = -x4*y3;  D(row, 10) = 0;     D(row, 11) = x2*y3;
    D(row, 12) = 0;       D(row, 13) = -x4*y4; D(row, 14) = 0;     D(row, 15) = x2*y4;
    ++row;
    D(row, 0) = 0;        D(row, 1) = 0;       D(row, 2) = -x4*y1;      D(row, 3) = x3*y1;
    D(row, 4) = 0;        D(row, 5) = 0;       D(row, 6) = -x4*y2;      D(row, 7) = x3*y2;
    D(row, 8) = 0;        D(row, 9) = 0;       D(row, 10) = -x4*y3;     D(row, 11) = x3*y3;
    D(row, 12) = 0;       D(row, 13) = 0;      D(row, 14) = -x4*y4;     D(row, 15) = x3*y4;
    ++row;
    D(row, 0) = 0;        D(row, 1) = -x3*y1;       D(row, 2) = x2*y1;      D(row, 3) = 0;
    D(row, 4) = 0;        D(row, 5) = -x3*y2;       D(row, 6) = x2*y2;      D(row, 7) = 0;
    D(row, 8) = 0;        D(row, 9) = -x3*y3;       D(row, 10) = x2*y3;     D(row, 11) = 0;
    D(row, 12) = 0;       D(row, 13) = -x3*y4;      D(row, 14) = x2*y4;     D(row, 15) = 0;
    ++row;
  }

  D.normalize_rows();
  vnl_svd<double> svd(D);

  //
  // FSM added :
  //
  if (svd.W(15)<DEGENERACY_THRESHOLD*svd.W(16)) {
    std::cerr << "vgl_h_matrix_3d_compute_linear : design matrix has rank < 16\n"
             << "vgl_h_matrix_3d_compute_linear : probably due to degenerate point configuration\n";
    return false;
  }
  // form the matrix from the nullvector
  H.set(svd.nullvector().data_block());
  return true;
}

bool vgl_h_matrix_3d_compute_linear::
compute_p(std::vector<vgl_homg_point_3d<double> > const& points1,
          std::vector<vgl_homg_point_3d<double> > const& points2,
          vgl_h_matrix_3d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int n = points1.size();

  if (n * 3 < TM_UNKNOWNS_COUNT - 1) {
    std::cerr << "vgl_h_matrix_3d_compute_linear: Need at least 5 matches.\n";
    if (n == 0) std::cerr << "Could be std::vector setlength idiosyncrasies!\n";
    return false;
  }

  //compute the normalizing transforms
  vgl_norm_trans_3d<double> tr1, tr2;
  if (!tr1.compute_from_points(points1))
    return false;
  if (!tr2.compute_from_points(points2))
    return false;
  std::vector<vgl_homg_point_3d<double> > tpoints1, tpoints2;
  for (int i = 0; i<n; i++)
  {
    tpoints1.push_back(tr1(points1[i]));
    tpoints2.push_back(tr2(points2[i]));
  }
  vgl_h_matrix_3d<double> hh(tpoints1, tpoints2);
  //vgl_h_matrix_3d<double> hh;
  //if (!solve_linear_problem(tpoints1,tpoints2,hh))
    //return false;

  //
  // Next, hh has to be transformed back to the coordinate system of
  // the original point sets, i.e.,
  //  p1' = tr1 p1 , p2' = tr2 p2
  // hh was determined from the transform relation
  //  p2' = hh p1', thus
  // (tr2 p2) = hh (tr1 p1)
  //  p2 = (tr2^-1 hh tr1) p1 = H p1
  vgl_h_matrix_3d<double> tr2_inv = tr2.get_inverse();
  H = tr2_inv*hh*tr1;
  return true;
}
