// This is brl/bbas/bvgl/bvgl_h_matrix_2d_compute_linear.cxx
#include "bvgl_h_matrix_2d_compute_linear.h"
//:
// \file

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vnl/algo/vnl_svd.h>
#include <bvgl/bvgl_norm_trans_2d.h>

//: Construct a bvgl_h_matrix_2d_compute_linear object.
// The allow_ideal_points flag is described below.
bvgl_h_matrix_2d_compute_linear::bvgl_h_matrix_2d_compute_linear(bool allow_ideal_points) :
  allow_ideal_points_(allow_ideal_points)
{
}


// Should provide:
//   Points-only method
//   Lines-only
//   Points&lines
//
// FSM - this is now done by bvgl_h_matrix_2d_compute_design.

const int TM_UNKNOWNS_COUNT = 9;
const double DEGENERACY_THRESHOLD = 0.00001;  // FSM. see below.

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
// for now, this function only works with points JLM
bool bvgl_h_matrix_2d_compute_linear::
compute_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
          vcl_vector<vgl_homg_point_2d<double> > const& points2,
          bvgl_h_matrix_2d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int n = points1.size();

  int equ_count = n * (allow_ideal_points_ ? 3 : 2);
  if (n * 2 < TM_UNKNOWNS_COUNT - 1) {
    vcl_cerr << "bvgl_h_matrix_2d_compute_linear: Need at least 4 matches.\n";
    if (n == 0) vcl_cerr << "Could be vcl_vector setlength idiosyncrasies!\n";
    return false;
  }
  //compute the normalizing transforms
  bvgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_points(points1))
    return false;
  if (!tr2.compute_from_points(points2))
    return false;

  //transform the point sets and fill the design matrix
  vnl_matrix<double> D(equ_count, TM_UNKNOWNS_COUNT);

  int row = 0;
  for (int i = 0; i < n; i++)
  {
    vgl_homg_point_2d<double> p1 = tr1(points1[i]);
    vgl_homg_point_2d<double> p2 = tr2(points2[i]);
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

    if (allow_ideal_points_)
    {
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
  if (svd.W(7) < DEGENERACY_THRESHOLD*svd.W(8)) {
    vcl_cerr << "bvgl_h_matrix_2d_compute_linear : design matrix has rank < 8\n"
             << "bvgl_h_matrix_2d_compute_linear : probably due to degenerate point configuration\n";
    return false;
  }
  // form the matrix from the nullvector
  bvgl_h_matrix_2d<double> hh;
  hh.set(svd.nullvector().data_block());
  //
  // Next, hh has to be transformed back to the coordinate system of
  // the original point sets, i.e.,
  //  p1' = tr1 p1 , p2' = tr2 p2
  // hh was detemined from the transform relation
  //  p2' = hh p1', thus
  // (tr2 p2) = hh (tr1 p1)
  //  p2 = (tr2^-1 hh tr1) p1 = H p1
  bvgl_h_matrix_2d<double> tr2_inv = tr2.get_inverse();
  H = tr2_inv*hh*tr1;
  return true;
}
