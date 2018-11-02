// This is oxl/mvl/HMatrix2DComputeLinear.cxx
#include <vector>
#include <iostream>
#include "HMatrix2DComputeLinear.h"
//:
//  \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/algo/vnl_svd.h>
#include <mvl/HMatrix2D.h>
#include <mvl/HomgMetric.h>
#include <mvl/HomgNorm2D.h>

//: Construct a HMatrix2DComputeLinear object.
// The allow_ideal_points flag is described below.
HMatrix2DComputeLinear::HMatrix2DComputeLinear(bool allow_ideal_points):
  allow_ideal_points_(allow_ideal_points)
{
}


// Should provide:
//   Points-only method
//   Lines-only
//   Points&lines
//
// FSM - this is now done by HMatrix2DComputeDesign.

constexpr int TM_UNKNOWNS_COUNT = 9;
constexpr double DEGENERACY_THRESHOLD = 0.00001;  // FSM. see below.

//-----------------------------------------------------------------------------
//
//: Compute a plane-plane projectivity using linear least squares.
// Returns false if the calculation fails or there are fewer than four point
// matches in the list.  The algorithm finds the nullvector of the $2 n \times 9$ design
// matrix:
// \f[
// \left(\begin{array}{ccccccccc}
// 0 & 0 & 0 &        x_1 z_1' & y_1 z_1' & z_1 z_1' & -x_1 y_1' & -y_1 y_1' & -z_1 y_1' \cr
// x_1 z_1' & y_1 z_1' & z_1 z_1' & 0 & 0 & 0 & -x_1 x_1' & -y_1 x_1' & -z_1 x_1' \cr
// \multicolumn{9}{c}{\cdots} \cr
// 0 & 0 & 0 &        x_n z_n' & y_n z_n' & z_n z_n' & -x_n y_n' & -y_n y_n' & -z_n y_n'\cr
// x_n z_n' & y_n z_n' & z_n z_n' & 0 & 0 & 0 & -x_n x_n' & -y_n x_n' & -z_n x_n'
// \end{array}\right)
// \f]
// If \t allow_ideal_points was set at construction, the $3 \times 9$ version which
// allows for ideal points is used.

bool
HMatrix2DComputeLinear::compute_p(PointArray const& inpoints1,
                                  PointArray const& inpoints2,
                                  HMatrix2D *H)
{
  // tm_tmatrix_linear_nonrobust_trivecs
  assert(inpoints1.size() == inpoints2.size());
  int n = inpoints1.size();

  int equ_count = n * (allow_ideal_points_ ? 3 : 2);
  if (n * 2 < TM_UNKNOWNS_COUNT - 1) {
    std::cerr << "HMatrix2DComputeLinear: Need at least 4 matches.\n";
    if (n == 0) std::cerr << "Could be std::vector setlength idiosyncrasies!\n";
    return false;
  }

  HomgNorm2D points1(inpoints1); if (points1.was_coincident()) return false; // FSM
  HomgNorm2D points2(inpoints2); if (points2.was_coincident()) return false; // FSM

  vnl_matrix<double> D(equ_count, TM_UNKNOWNS_COUNT);

  int row = 0;
  for (int i = 0; i < n; i++) {
    const HomgPoint2D& p1 = points1[i];
    const HomgPoint2D& p2 = points2[i];

    D(row, 0) =  p1.x() * p2.w();
    D(row, 1) =  p1.y() * p2.w();
    D(row, 2) =  p1.w() * p2.w();
    D(row, 3) = 0;
    D(row, 4) = 0;
    D(row, 5) = 0;
    D(row, 6) = -p1.x() * p2.x();
    D(row, 7) = -p1.y() * p2.x();
    D(row, 8) = -p1.w() * p2.x();
    ++row;

    D(row, 0) = 0;
    D(row, 1) = 0;
    D(row, 2) = 0;
    D(row, 3) =  p1.x() * p2.w();
    D(row, 4) =  p1.y() * p2.w();
    D(row, 5) =  p1.w() * p2.w();
    D(row, 6) = -p1.x() * p2.y();
    D(row, 7) = -p1.y() * p2.y();
    D(row, 8) = -p1.w() * p2.y();
    ++row;

    if (allow_ideal_points_) {
      D(row, 0) =  p1.x() * p2.y();
      D(row, 1) =  p1.y() * p2.y();
      D(row, 2) =  p1.w() * p2.y();
      D(row, 3) = -p1.x() * p2.x();
      D(row, 4) = -p1.y() * p2.x();
      D(row, 5) = -p1.w() * p2.x();
      D(row, 6) = 0;
      D(row, 7) = 0;
      D(row, 8) = 0;
      ++row;
    }
  }

  D.normalize_rows();
  vnl_svd<double> svd(D);

  //
  // FSM added :
  //
  if (svd.W(7)<DEGENERACY_THRESHOLD*svd.W(8)) {
    std::cerr << "HMatrix2DComputeLinear : design matrix has rank < 8" << std::endl;
    std::cerr << "HMatrix2DComputeLinear : probably due to degenerate point configuration" << std::endl;
    return false;
  }
  H->set(svd.nullvector().data_block());

  *H = HomgMetric::homg_to_image_H(*H, &points1, &points2);

  return true;
}
