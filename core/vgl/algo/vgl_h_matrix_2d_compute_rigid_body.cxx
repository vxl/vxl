// This is core/vgl/algo/vgl_h_matrix_2d_compute_rigid_body.cxx
#include <iostream>
#include <cmath>
#include "vgl_h_matrix_2d_compute_rigid_body.h"
//:
// \file

#include <vcl_compiler.h>
#include <cassert>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/algo/vgl_norm_trans_2d.h>

//: Construct a vgl_h_matrix_2d_compute_rigid_body object.
vgl_h_matrix_2d_compute_rigid_body::vgl_h_matrix_2d_compute_rigid_body() = default;

constexpr int TM_UNKNOWNS_COUNT = 3;
constexpr double DEGENERACY_THRESHOLD = 0.01;

//-----------------------------------------------------------------------------
//
//: Compute the rigid body transformation between point sets
//
bool vgl_h_matrix_2d_compute_rigid_body::
solve_rigid_body_problem(int equ_count,
                         std::vector<vgl_homg_point_2d<double> > const& p1,
                         std::vector<vgl_homg_point_2d<double> > const& p2,
                         vgl_h_matrix_2d<double>& H)
{
  //transform the point sets and fill the design matrix
  vnl_matrix<double> D(equ_count, TM_UNKNOWNS_COUNT+2);
  int n = p1.size();
  int row = 0;
  for (int i = 0; i < n; i++) {
    D(row, 0) =  p1[i].x();
    D(row, 1) =  -p1[i].y();
    D(row, 2) =  1.0;
    D(row, 3) =  0.0;
    D(row, 4) = -p2[i].x();
    ++row;

    D(row, 0) = p1[i].y();
    D(row, 1) = p1[i].x();
    D(row, 2) = 0;
    D(row, 3) =  1.0;
    D(row, 4) =  -p2[i].y();
    ++row;
  }

  D.normalize_rows();
  vnl_svd<double> svd(D);
  vnl_vector<double> nullv = svd.nullvector();
  //last singular value should be zero for ideal data
  if (svd.W(4)>DEGENERACY_THRESHOLD*svd.W(3)) {
            std::cout  << "vgl_h_matrix_2d_compute_rigid_body : inaccurate solution probably due to inconsistent point correspondences\n"
                      << "W\n" << svd.W() << std::endl;
    return false;
  }

  // form the matrix from the nullvector
  // normalize by the last value
  double norm = nullv[4];
  nullv /= norm;
  // convert to rotation
  double y = nullv[1];
  double x = nullv[0];
  double angle = std::atan2(y,x);
  double c = std::cos(angle);
  double s = std::sin(angle);
  vnl_matrix_fixed<double, 3,3> M;
  M[0][0]=c;   M[0][1]= -s; M[0][2] = nullv[2];
  M[1][0]=s;   M[1][1]= c; M[1][2] = nullv[3];
  M[2][0]=0;   M[2][1]= 0; M[2][2] = 1;
  H.set(M);
  return true;
}

bool vgl_h_matrix_2d_compute_rigid_body::
compute_p(std::vector<vgl_homg_point_2d<double> > const& points1,
          std::vector<vgl_homg_point_2d<double> > const& points2,
          vgl_h_matrix_2d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int n = points1.size();

  int equ_count = n * (2);
  if (n * 2 < TM_UNKNOWNS_COUNT) {
    std::cerr << "vgl_h_matrix_2d_compute_rigid_body: Need at least 2 matches.\n";
    if (n == 0) std::cerr << "Could be std::vector setlength idiosyncrasies!\n";
    return false;
  }
  //compute the normalizing transforms
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_points(points1))
    return false;
  if (!tr2.compute_from_points(points2))
    return false;
  std::vector<vgl_homg_point_2d<double> > tpoints1, tpoints2;
  for (int i = 0; i<n; i++)
  {
    tpoints1.push_back(tr1(points1[i]));
    tpoints2.push_back(tr2(points2[i]));
  }
  vgl_h_matrix_2d<double> hh;
  if (!solve_rigid_body_problem(equ_count, tpoints1, tpoints2, hh))
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

bool vgl_h_matrix_2d_compute_rigid_body::
compute_l(std::vector<vgl_homg_line_2d<double> > const& lines1,
          std::vector<vgl_homg_line_2d<double> > const& lines2,
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
  std::vector<vgl_homg_point_2d<double> > tlines1, tlines2;
  for (const auto & lit : lines1)
  {
    // transform the lines according to the normalizing transform
    vgl_homg_line_2d<double> l = tr1(lit);
    // convert the line to a point to use the same rigid_body code
    vgl_homg_point_2d<double> p(l.a(), l.b(), l.c());
    tlines1.push_back(p);
  }
  for (const auto & lit : lines2)
  {
    // transform the lines according to the normalizing transform
    vgl_homg_line_2d<double> l = tr2(lit);
    // convert the line to a point to use the same rigid_body code
    vgl_homg_point_2d<double> p(l.a(), l.b(), l.c());
    tlines2.push_back(p);
  }

  vgl_h_matrix_2d<double> hl,hp,tr2inv;
  if (!solve_rigid_body_problem(equ_count, tlines1, tlines2, hl))
    return false;
  // The result is a transform on lines so we need to convert it to
  // a point transform, i.e., hp = hl^-t.
  vnl_matrix_fixed<double, 3, 3> const &  Ml = hl.get_matrix();
  vnl_matrix_fixed<double, 3, 3> Mp = vnl_inverse_transpose(Ml);
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

bool vgl_h_matrix_2d_compute_rigid_body::
compute_pl(std::vector<vgl_homg_point_2d<double> > const& points1,
           std::vector<vgl_homg_point_2d<double> > const& points2,
           std::vector<vgl_homg_line_2d<double> > const& lines1,
           std::vector<vgl_homg_line_2d<double> > const& lines2,
           vgl_h_matrix_2d<double>& H)
{
  //number of points must be the same
  assert(points1.size() == points2.size());
  int np = points1.size();
  //number of lines must be the same
  assert(lines1.size() == lines2.size());
  int nl = lines1.size();

  int equ_count = np * 2 + 2*nl;
  if ((np+nl)*2 < TM_UNKNOWNS_COUNT)
  {
    std::cerr << "vgl_h_matrix_2d_compute_rigid_body: Need at least 4 matches.\n";
    if (np+nl == 0) std::cerr << "Could be std::vector setlength idiosyncrasies!\n";
    return false;
  }
  //compute the normalizing transforms
  vgl_norm_trans_2d<double> tr1, tr2;
  if (!tr1.compute_from_points_and_lines(points1,lines1))
    return false;
  if (!tr2.compute_from_points_and_lines(points2,lines2))
    return false;
  std::vector<vgl_homg_point_2d<double> > tpoints1, tpoints2;
  for (int i = 0; i<np; i++)
  {
    tpoints1.push_back(tr1(points1[i]));
    tpoints2.push_back(tr2(points2[i]));
  }
  for (int i = 0; i<nl; i++)
  {
    double a=lines1[i].a(), b=lines1[i].b(), c=lines1[i].c(), d=std::sqrt(a*a+b*b);
    tpoints1.push_back(tr1(vgl_homg_point_2d<double>(-a*c,-b*c,d)));
    a=lines2[i].a(), b=lines2[i].b(), c=lines2[i].c(), d = std::sqrt(a*a+b*b);
    tpoints2.push_back(tr2(vgl_homg_point_2d<double>(-a*c,-b*c,d)));
  }
  vgl_h_matrix_2d<double> hh;
  if (!solve_rigid_body_problem(equ_count, tpoints1, tpoints2, hh))
    return false;

  vgl_h_matrix_2d<double> tr2_inv = tr2.get_inverse();
  H = tr2_inv*hh*tr1;
  return true;
}

bool vgl_h_matrix_2d_compute_rigid_body::
compute_l(std::vector<vgl_homg_line_2d<double> > const& /* lines1 */,
          std::vector<vgl_homg_line_2d<double> > const& /* lines2 */,
          std::vector<double> const& /* weights */,
          vgl_h_matrix_2d<double>& /* H */)
{
  return false;
}

#if 0 // do later
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
bool vgl_h_matrix_2d_compute_rigid_body::
solve_weighted_least_squares(std::vector<vgl_homg_line_2d<double> > const& l1,
                             std::vector<vgl_homg_line_2d<double> > const& l2,
                             std::vector<double> const& w,
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
  M = vnl_transpose(D)*W*D;
  D.normalize_rows();
  vnl_svd<double> svd(D);

  //
  // FSM added :
  //
  if (svd.W(7)<DEGENERACY_THRESHOLD*svd.W(8)) {
    std::cerr << "vgl_h_matrix_2d_compute_rigid_body : design matrix has rank < 8\n"
             << "vgl_h_matrix_2d_compute_rigid_body : probably due to degenerate point configuration\n";
    return false;
  }
  // form the matrix from the nullvector
  H.set(svd.nullvector().data_block());
  return true;
}

bool vgl_h_matrix_2d_compute_rigid_body::
compute_l(std::vector<vgl_homg_line_2d<double> > const& lines1,
          std::vector<vgl_homg_line_2d<double> > const& lines2,
          std::vector<double> const & weights,
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
  std::vector<vgl_homg_line_2d<double> > tlines1, tlines2;
  for (std::vector<vgl_homg_line_2d<double> >::const_iterator
       lit = lines1.begin(); lit != lines1.end(); lit++)
  {
    // transform the lines according to the normalizing transform
    vgl_homg_line_2d<double> l = tr1(*lit);
    tlines1.push_back(l);
  }
  for (std::vector<vgl_homg_line_2d<double> >::const_iterator
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
  vnl_matrix_fixed<double, 3, 3> Mp = vnl_inverse_transpose(Ml);
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
#endif // 0
