// This is core/vgl/algo/vgl_h_matrix_2d_compute_linear.cxx
#include "vgl_h_matrix_2d_compute_linear.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/algo/vgl_norm_trans_2d.h>

//: Construct a vgl_h_matrix_2d_compute_linear object.
// The allow_ideal_points flag is described below.
vgl_h_matrix_2d_compute_linear::vgl_h_matrix_2d_compute_linear(bool allow_ideal_points):
  allow_ideal_points_(allow_ideal_points)
{
}


// Should provide:
//   Points-only method
//   Lines-only
//   Points&lines
//
// FSM - this is now done by vgl_h_matrix_2d_compute_design.

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
bool vgl_h_matrix_2d_compute_linear::
solve_linear_problem(int equ_count,
                     vcl_vector<vgl_homg_point_2d<double> > const& p1,
                     vcl_vector<vgl_homg_point_2d<double> > const& p2,
                     vgl_h_matrix_2d<double>& H)
{
  //transform the point sets and fill the design matrix
  vnl_matrix<double> D(equ_count, TM_UNKNOWNS_COUNT);
  int n = p1.size();
  int row = 0;
  for (int i = 0; i < n; i++) {
    D(row, 0) =  p1[i].x() * p2[i].w();
    D(row, 1) =  p1[i].y() * p2[i].w();
    D(row, 2) =  p1[i].w() * p2[i].w();
    D(row, 3) = 0;
    D(row, 4) = 0;
    D(row, 5) = 0;
    D(row, 6) = -p1[i].x() * p2[i].x();
    D(row, 7) = -p1[i].y() * p2[i].x();
    D(row, 8) = -p1[i].w() * p2[i].x();
    ++row;

    D(row, 0) = 0;
    D(row, 1) = 0;
    D(row, 2) = 0;
    D(row, 3) =  p1[i].x() * p2[i].w();
    D(row, 4) =  p1[i].y() * p2[i].w();
    D(row, 5) =  p1[i].w() * p2[i].w();
    D(row, 6) = -p1[i].x() * p2[i].y();
    D(row, 7) = -p1[i].y() * p2[i].y();
    D(row, 8) = -p1[i].w() * p2[i].y();
    ++row;

    if (allow_ideal_points_) {
      D(row, 0) =  p1[i].x() * p2[i].y();
      D(row, 1) =  p1[i].y() * p2[i].y();
      D(row, 2) =  p1[i].w() * p2[i].y();
      D(row, 3) = -p1[i].x() * p2[i].x();
      D(row, 4) = -p1[i].y() * p2[i].x();
      D(row, 5) = -p1[i].w() * p2[i].x();
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
    vcl_cerr << "vgl_h_matrix_2d_compute_linear : design matrix has rank < 8\n"
             << "vgl_h_matrix_2d_compute_linear : probably due to degenerate point configuration\n";
    return false;
  }
  // form the matrix from the nullvector
  H.set(svd.nullvector().data_block());
  return true;
}

bool vgl_h_matrix_2d_compute_linear::
compute_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
          vcl_vector<vgl_homg_point_2d<double> > const& points2,
          vgl_h_matrix_2d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int n = points1.size();

  int equ_count = n * (allow_ideal_points_ ? 3 : 2);
  if (n * 2 < TM_UNKNOWNS_COUNT - 1) {
    vcl_cerr << "vgl_h_matrix_2d_compute_linear: Need at least 4 matches.\n";
    if (n == 0) vcl_cerr << "Could be vcl_vector setlength idiosyncrasies!\n";
    return false;
  }
  //compute the normalizing transforms
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_points(points1))
    return false;
  if (!tr2.compute_from_points(points2))
    return false;
  vcl_vector<vgl_homg_point_2d<double> > tpoints1, tpoints2;
  for (int i = 0; i<n; i++)
  {
    tpoints1.push_back(tr1(points1[i]));
    tpoints2.push_back(tr2(points2[i]));
  }
  vgl_h_matrix_2d<double> hh;
  if (!solve_linear_problem(equ_count, tpoints1, tpoints2, hh))
    return false;
  //
  // Next, hh has to be transformed back to the coordinate system of
  // the original point sets, i.e.,
  //  p1' = tr1 p1 , p2' = tr2 p2
  // hh was determined from the transform relation
  //  p2' = hh p1', thus
  // (tr2 p2) = hh (tr1 p1)
  //  p2 = (tr2^-1 hh tr1) p1 = H p1
  vgl_h_matrix_2d<double> tr2_inv = tr2.get_inverse();
  H = tr2_inv*hh*tr1;
  return true;
}

bool vgl_h_matrix_2d_compute_linear::
compute_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
          vcl_vector<vgl_homg_line_2d<double> > const& lines2,
          vgl_h_matrix_2d<double>& H)
{
  //number of lines must be the same
  assert(lines1.size() == lines2.size());
  int n = lines1.size();
  int equ_count = 2*n;
  //compute the normalizing transforms. By convention, these are point
  //transformations.
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_lines(lines1))
    return false;
  if (!tr2.compute_from_lines(lines2))
    return false;
  vcl_vector<vgl_homg_point_2d<double> > tlines1, tlines2;
  for (vcl_vector<vgl_homg_line_2d<double> >::const_iterator
       lit = lines1.begin(); lit != lines1.end(); lit++)
  {
    // transform the lines according to the normalizing transform
    vgl_homg_line_2d<double> l = tr1(*lit);
    // convert the line to a point to use the same linear code
    vgl_homg_point_2d<double> p(l.a(), l.b(), l.c());
    tlines1.push_back(p);
  }
  for (vcl_vector<vgl_homg_line_2d<double> >::const_iterator
       lit = lines2.begin(); lit != lines2.end(); lit++)
  {
    // transform the lines according to the normalizing transform
    vgl_homg_line_2d<double> l = tr2(*lit);
    // convert the line to a point to use the same linear code
    vgl_homg_point_2d<double> p(l.a(), l.b(), l.c());
    tlines2.push_back(p);
  }

  vgl_h_matrix_2d<double> hl,hp,tr2inv;
  if (!solve_linear_problem(equ_count, tlines1, tlines2, hl))
    return false;
  // The result is a transform on lines so we need to convert it to
  // a point transform, i.e., hp = hl^-t.
  vnl_matrix_fixed<double, 3, 3> const &  Ml = hl.get_matrix();
  vnl_matrix_fixed<double, 3, 3> Mp = (vnl_inverse(Ml)).transpose();
  hp.set(Mp);
  //
  // Next, hp has to be transformed back to the coordinate system of
  // the original lines, i.e.,
  //  l1' = tr1 l1 , l2' = tr2 l2
  // hp was determined from the transform relation
  //  l2' = hh l1', thus
  // (tr2 l2) = hh (tr1 l1)
  //  p2 = (tr2^-1 hh tr1) p1 = H p1
  tr2inv = tr2.get_inverse();
  H = tr2inv*hp*tr1;
  return true;
}

bool vgl_h_matrix_2d_compute_linear::
compute_pl(vcl_vector<vgl_homg_point_2d<double> > const& points1,
           vcl_vector<vgl_homg_point_2d<double> > const& points2,
           vcl_vector<vgl_homg_line_2d<double> > const& lines1,
           vcl_vector<vgl_homg_line_2d<double> > const& lines2,
           vgl_h_matrix_2d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int np = points1.size();
  //number of lines must be the same
  assert(lines1.size() == lines2.size());
  int nl = lines1.size();

  int equ_count = np * (allow_ideal_points_ ? 3 : 2) + 2*nl;
  if ((np+nl)*2+1 < TM_UNKNOWNS_COUNT)
  {
    vcl_cerr << "vgl_h_matrix_2d_compute_linear: Need at least 4 matches.\n";
    if (np+nl == 0) vcl_cerr << "Could be vcl_vector setlength idiosyncrasies!\n";
    return false;
  }
  //compute the normalizing transforms
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_points_and_lines(points1,lines1))
    return false;
  if (!tr2.compute_from_points_and_lines(points2,lines2))
    return false;
  vcl_vector<vgl_homg_point_2d<double> > tpoints1, tpoints2;
  for (int i = 0; i<np; i++)
  {
    tpoints1.push_back(tr1(points1[i]));
    tpoints2.push_back(tr2(points2[i]));
  }
  for (int i = 0; i<nl; i++)
  {
    double a=lines1[i].a(), b=lines1[i].b(), c=lines1[i].c(), d=vcl_sqrt(a*a+b*b);
    tpoints1.push_back(tr1(vgl_homg_point_2d<double>(-a*c,-b*c,d)));
    a=lines2[i].a(), b=lines2[i].b(), c=lines2[i].c(), d = vcl_sqrt(a*a+b*b);
    tpoints2.push_back(tr2(vgl_homg_point_2d<double>(-a*c,-b*c,d)));
  }
  vgl_h_matrix_2d<double> hh;
  if (!solve_linear_problem(equ_count, tpoints1, tpoints2, hh))
    return false;

  vgl_h_matrix_2d<double> tr2_inv = tr2.get_inverse();
  H = tr2_inv*hh*tr1;
  return true;
}

//--------------------------------------------------------
//:
//  The solution equations should be weighted by the length of
//  the corresponding line matches.  This weighting is given by w.
//
//  The two equations resulting from l1i<->l2i should be
//  weighted by wi.  Form a m x m diagonal matrix W with elements from w,
//  with m = 2*Nc, where Nc=l1.size()=l2.size() is the number of
//  corresponding line pairs.  The weighted least squares problem is
//  expressed as:
//
//               (D^tWD)x = Mx = 0
//
//  where D is the design matrix and x is the 9 element vector of unknown
//  homography matrix elements. This problem can be solved using SVD as in the
//  case of unweighted least squares.
//
bool vgl_h_matrix_2d_compute_linear::
solve_weighted_least_squares(vcl_vector<vgl_homg_line_2d<double> > const& l1,
                             vcl_vector<vgl_homg_line_2d<double> > const& l2,
                             vcl_vector<double> const& w,
                             vgl_h_matrix_2d<double>& H)
{
  int Nc = l1.size();
  // Note the w has size Nc so we need to form a 2*Nc vector with
  // repeated values
  vnl_vector<double> two_w(2*Nc);
  int j =0;
  for (int i = 0; i<Nc; i++, j+=2)
  {
    two_w[j]=w[i];
    two_w[j+1]=w[i];
  }
  vnl_diag_matrix<double> W(two_w);

  //Form the design matrix, D
  vnl_matrix<double> D(2*Nc, TM_UNKNOWNS_COUNT);
  vnl_matrix<double> M(TM_UNKNOWNS_COUNT, TM_UNKNOWNS_COUNT);

  int row = 0;
  for (int i = 0; i < Nc; i++)
  {
    D(row, 0) =  l1[i].a() * l2[i].c();
    D(row, 1) =  l1[i].b() * l2[i].c();
    D(row, 2) =  l1[i].c() * l2[i].c();
    D(row, 3) = 0;
    D(row, 4) = 0;
    D(row, 5) = 0;
    D(row, 6) = -l1[i].a() * l2[i].a();
    D(row, 7) = -l1[i].b() * l2[i].a();
    D(row, 8) = -l1[i].c() * l2[i].a();
    ++row;

    D(row, 0) = 0;
    D(row, 1) = 0;
    D(row, 2) = 0;
    D(row, 3) =  l1[i].a() * l2[i].c();
    D(row, 4) =  l1[i].b() * l2[i].c();
    D(row, 5) =  l1[i].c() * l2[i].c();
    D(row, 6) = -l1[i].a() * l2[i].b();
    D(row, 7) = -l1[i].b() * l2[i].b();
    D(row, 8) = -l1[i].c() * l2[i].b();
    ++row;
  }
  M = D.transpose()*W*D;
  D.normalize_rows();
  vnl_svd<double> svd(D);

  //
  // FSM added :
  //
  if (svd.W(7)<DEGENERACY_THRESHOLD*svd.W(8)) {
    vcl_cerr << "vgl_h_matrix_2d_compute_linear : design matrix has rank < 8\n"
             << "vgl_h_matrix_2d_compute_linear : probably due to degenerate point configuration\n";
    return false;
  }
  // form the matrix from the nullvector
  H.set(svd.nullvector().data_block());
  return true;
}

bool vgl_h_matrix_2d_compute_linear::
compute_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
          vcl_vector<vgl_homg_line_2d<double> > const& lines2,
          vcl_vector<double> const & weights,
          vgl_h_matrix_2d<double>& H)
{
  //number of lines must be the same
  assert(lines1.size() == lines2.size());
//int n = lines1.size();
  //compute the normalizing transforms. By convention, these are point
  //transformations.
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_lines(lines1))
    return false;
  if (!tr2.compute_from_lines(lines2))
    return false;
  vcl_vector<vgl_homg_line_2d<double> > tlines1, tlines2;
  for (vcl_vector<vgl_homg_line_2d<double> >::const_iterator
       lit = lines1.begin(); lit != lines1.end(); lit++)
  {
    // transform the lines according to the normalizing transform
    vgl_homg_line_2d<double> l = tr1(*lit);
    tlines1.push_back(l);
  }
  for (vcl_vector<vgl_homg_line_2d<double> >::const_iterator
       lit = lines2.begin(); lit != lines2.end(); lit++)
  {
    // transform the lines according to the normalizing transform
    vgl_homg_line_2d<double> l = tr2(*lit);
    tlines2.push_back(l);
  }

  vgl_h_matrix_2d<double> hl,hp,tr2inv;
  if (!solve_weighted_least_squares(tlines1, tlines2, weights, hl))
    return false;
  // The result is a transform on lines so we need to convert it to
  // a point transform, i.e., hp = hl^-t.
  vnl_matrix_fixed<double, 3, 3> const &  Ml = hl.get_matrix();
  vnl_matrix_fixed<double, 3, 3> Mp = (vnl_inverse(Ml)).transpose();
  hp.set(Mp);
  //
  // Next, hp has to be transformed back to the coordinate system of
  // the original lines, i.e.,
  //  l1' = tr1 l1 , l2' = tr2 l2
  // hp was determined from the transform relation
  //  l2' = hh l1', thus
  // (tr2 l2) = hh (tr1 l1)
  //  p2 = (tr2^-1 hh tr1) p1 = H p1
  tr2inv = tr2.get_inverse();
  H = tr2inv*hp*tr1;
  return true;
}
