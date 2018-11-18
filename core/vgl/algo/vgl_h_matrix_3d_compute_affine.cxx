// This is core/vgl/algo/vgl_h_matrix_3d_compute_affine.cxx
#include <iostream>
#include <cmath>
#include <iomanip>
#include "vgl_h_matrix_3d_compute_affine.h"
//:
// \file

#include <cassert>
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/algo/vnl_svd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

constexpr int TM_UNKNOWNS_COUNT = 9;
constexpr double DEGENERACY_THRESHOLD = 0.00001;


//: Compute a 3D-to-3D affine transformation without translation 3 x 3 matrix
// Note: requires all finite points.
// The algorithm finds the nullvector of the $3 n \times 9$ design matrix.

//:Assumes all corresponding points have equal weight
bool vgl_h_matrix_3d_compute_affine::
solve_linear_problem(std::vector<vgl_homg_point_3d<double> > const& p1,
                     std::vector<vgl_homg_point_3d<double> > const& p2,
                     vnl_matrix<double>& M)
{
  int n = static_cast<int>(p1.size());
  //fill the design matrix
  vnl_matrix<double> D(n*3, TM_UNKNOWNS_COUNT, 0.0);
  vnl_matrix<double> b(n*3, 1);

  int row = 0;
  for (int i = 0; i < n; i++) {
    vgl_homg_point_3d<double> hp1 = p1[i], hp2 = p2[i];
    hp1.rescale_w(); hp2.rescale_w();
    double X1 = hp1.x(), X2 = hp1.y(), X3 = hp1.z();
    double x1 = hp2.x(), x2 = hp2.y(), x3 = hp2.z();
#if 0
    std::cout << "X(" << X1 << ' ' << X2 << ' ' << X3 << '\n';
    std::cout << "x(" << x1 << ' ' << x2 << ' ' << x3 << '\n';
#endif
    D(row, 0) =   X1;   D(row, 1) =   X2;   D(row, 2) =   X3;  b(row,0) = x1;
    D(row+1, 3) = X1;   D(row+1, 4) = X2;   D(row+1, 5) = X3;  b(row+1,0) = x2;
    D(row+2, 6) = X1;   D(row+2, 7) = X2;   D(row+2, 8) = X3;  b(row+2,0) = x3;
    row+=3;
  }
#if 0
  std::cout << '{';
  for(int r = 0; r<(3*n); ++r){
    std::cout << '{';
    for(int c = 0; c<9; ++c)
      std::cout << D[r][c] << ',';
    std::cout << "},";
  }
  std::cout << '}';
  std::cout << '{';
  for(int r = 0; r<(3*n); ++r)
    std::cout << b[r][0] << ',';
  std::cout << "}\n";
#endif
  vnl_svd<double> svd(D);
  std::cout << svd.W() << '\n';
  if (svd.W(8)<DEGENERACY_THRESHOLD*svd.W(7)) {
    std::cerr << "vgl_h_matrix_3d_compute_linear : design matrix has rank < 9\n"
             << "vgl_h_matrix_3d_compute_linear : probably due to degenerate point configuration\n";
    return false;
  }
  M = svd.solve(b);
  return true;
}

bool vgl_h_matrix_3d_compute_affine::
compute_p(std::vector<vgl_homg_point_3d<double> > const& points1,
          std::vector<vgl_homg_point_3d<double> > const& points2,
          vgl_h_matrix_3d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int n = static_cast<int>(points1.size());

  if (n * 3 < TM_UNKNOWNS_COUNT+3) {
    std::cerr << "vgl_h_matrix_3d_compute_affine: Need at least 4 matches.\n";
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
  vnl_matrix<double> M;
  if (!solve_linear_problem(tpoints1,tpoints2,M))
    return false;
  // form the h_matrix
  vnl_matrix_fixed<double, 4, 4> m;
  m.fill(0.0);
  m[0][0]=M[0][0]; m[0][1]=M[1][0]; m[0][2] = M[2][0];
  m[1][0]=M[3][0]; m[1][1]=M[4][0]; m[1][2] = M[5][0];
  m[2][0]=M[6][0]; m[2][1]=M[7][0]; m[2][2] = M[8][0];
  m[3][3] = 1.0;
#if 0
  for(unsigned r = 0; r<4; ++r)
    std::cout << std::setprecision(2) << m[r][0] << ' ' << m[r][1] << ' '<< m[r][2] << ' '<< m[r][3] << '\n';
  std::cout << '\n';
#endif
  vgl_h_matrix_3d<double> hh(m);
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
