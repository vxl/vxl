// This is oxl/mvl/HMatrix2DAffineCompute.cxx
//:
//  \file

#include <iostream>
#include <vector>
#include "HMatrix2DAffineCompute.h"
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vnl/vnl_matrix.h>
#include <mvl/HMatrix2D.h>
#include <mvl/HomgPoint2D.h>
#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_inverse.h>

#include <mvl/PairMatchSetCorner.h>

// Compute the 2D affine transformation (the actual implementation)
//
static bool tmp_fun(std::vector<vgl_homg_point_2d<double> > const& pts1,
                    std::vector<vgl_homg_point_2d<double> > const& pts2,
                    HMatrix2D& H)
{
  // Points on the affine manifold in the joint image satisfy
  // [A -I] * p + t = 0,
  // where p = [x1 ; x2] is a 4-vector, and A is 2x2

  assert(pts1.size() == pts2.size());

  NonHomg p1(pts1);
  NonHomg p2(pts2);
  vnl_double_2 mn1 = mean2(p1);
  vnl_double_2 mn2 = mean2(p2);
  sub_rows(p1,mn1);
  sub_rows(p2,mn2);

#if 0
  vnl_matrix<double> joint = vnl_matops().cat(p1,p2);// this doesn't work in 3.0
#else
  vnl_matrix<double> joint(pts1.size(),2+2);
  joint.update(p1,0,0);
  joint.update(p2,0,2);
#endif

  vnl_svd<double> svd(joint);
  // there are 2 4-element nullvectors, let [N M] = [n1 ; n2]
  vnl_double_2x2 N = (svd.V().extract(2,2,0,2)).transpose();
  vnl_double_2x2 M = (svd.V().extract(2,2,2,2)).transpose();

  vnl_double_2x2 Aff = - vnl_inverse(M) * N;
  vnl_double_2 t = mn2 - Aff*mn1;

  vnl_double_3x3 T;
  T.set_identity();
  T.update(Aff.as_ref());
  T(0,2) = t[0];
  T(1,2) = t[1];
  H.set(T);
  return true;
}

static bool tmp_fun(std::vector<HomgPoint2D> const& pts1,
                    std::vector<HomgPoint2D> const& pts2,
                    HMatrix2D& H)
{
  // Points on the affine manifold in the joint image satisfy
  // [A -I] * p + t = 0,
  // where p = [x1 ; x2] is a 4-vector, and A is 2x2

  assert(pts1.size() == pts2.size());

  NonHomg p1(pts1);
  NonHomg p2(pts2);
  vnl_double_2 mn1 = mean2(p1);
  vnl_double_2 mn2 = mean2(p2);
  sub_rows(p1,mn1);
  sub_rows(p2,mn2);

#if 0
  vnl_matrix<double> joint = vnl_matops().cat(p1,p2);// this doesn't work in 3.0
#else
  vnl_matrix<double> joint(pts1.size(),2+2);
  joint.update(p1,0,0);
  joint.update(p2,0,2);
#endif

  vnl_svd<double> svd(joint);
  // there are 2 4-element nullvectors, let [N M] = [n1 ; n2]
  vnl_double_2x2 N = (svd.V().extract(2,2,0,2)).transpose();
  vnl_double_2x2 M = (svd.V().extract(2,2,2,2)).transpose();

  vnl_double_2x2 Aff = - vnl_inverse(M) * N;
  vnl_double_2 t = mn2 - Aff*mn1;

  vnl_double_3x3 T;
  T.set_identity();
  T.update(Aff.as_ref());
  T(0,2) = t[0];
  T(1,2) = t[1];
  H.set(T);
  return true;
}

HMatrix2D
HMatrix2DAffineCompute::compute(const PairMatchSetCorner &matches)
{
 std::vector<HomgPoint2D> pts1(matches.count());
 std::vector<HomgPoint2D> pts2(matches.count());
 matches.extract_matches(pts1, pts2);
 HMatrix2D H;
 tmp_fun(pts1,pts2,H);
 return H;
}

HMatrix2D
HMatrix2DAffineCompute::compute(const std::vector<vgl_homg_point_2d<double> >&p1,
                                const std::vector<vgl_homg_point_2d<double> >&p2)
{
  HMatrix2D H;
  tmp_fun(p1,p2,H);
  return H;
}

HMatrix2D
HMatrix2DAffineCompute::compute(const std::vector<HomgPoint2D>&p1,
                                const std::vector<HomgPoint2D>&p2)
{
  HMatrix2D H;
  tmp_fun(p1,p2,H);
  return H;
}

bool
HMatrix2DAffineCompute::compute_p(const std::vector<HomgPoint2D> &pts1,
                                  const std::vector<HomgPoint2D> &pts2,
                                  HMatrix2D *H)
{
  return tmp_fun(pts1,pts2,*H);
}

//--------------------------------------------------------------------------------

NonHomg::NonHomg(std::vector<vgl_homg_point_2d<double> > const& A)
  : vnl_matrix<double>(A.size(),2)
{
  vnl_matrix<double> &X = *this;
  int n = rows();
  for (int i=0; i<n; ++i)
    X(i,0) = A[i].x()/A[i].w(),
    X(i,1) = A[i].y()/A[i].w();
}

NonHomg::NonHomg(const std::vector<HomgPoint2D> &A)
  : vnl_matrix<double>(A.size(),2)
{
  vnl_matrix<double> &X = *this;
  int n = rows();
  for (int i=0; i<n; ++i)
    A[i].get_nonhomogeneous(X(i,0),X(i,1));
}

vnl_double_2 mean2(const vnl_matrix<double> &A)
{
  assert(A.columns() == 2);
  vnl_double_2 mean(0,0);
  int n = A.rows();
  for (int j=0; j<2; ++j) {
    for (int k=0; k<n; ++k)
      mean[j] += A(k,j);
  }
  mean *= (1.0/n);
  return mean;
}

vnl_matrix<double>& sub_rows(vnl_matrix<double> &A, const vnl_double_2 a)
{
  unsigned c = A.columns();
  unsigned r = A.rows();
  assert(c == a.size());
  for (unsigned j=0; j < c; ++j)
    for (unsigned i=0; i < r; ++i)
      A(i,j) -= a[j];

  return A;
}
