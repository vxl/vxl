// This is oxl/mvl/HMatrix2DAffineCompute.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vnl/vnl_matrix.h>
#include <mvl/HMatrix2D.h>
#include <mvl/HomgPoint2D.h>
#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include "HMatrix2DAffineCompute.h"

//
//
//
#include <mvl/PairMatchSetCorner.h>
HMatrix2D
HMatrix2DAffineCompute::compute(const PairMatchSetCorner &matches)
{
 vcl_vector<HomgPoint2D> pts1(matches.count());
 vcl_vector<HomgPoint2D> pts2(matches.count());
 matches.extract_matches(pts1, pts2);
 HMatrix2D H;
 tmp_fun(pts1,pts2,&H);
 return H;
}
HMatrix2D
HMatrix2DAffineCompute::compute(const vcl_vector<HomgPoint2D>&p1,
                                const vcl_vector<HomgPoint2D>&p2)
{
  HMatrix2D H;
  tmp_fun(p1,p2,&H);
  return H;
}
bool
HMatrix2DAffineCompute::compute_p(const PointArray &pts1,
                                  const PointArray &pts2,
                                  HMatrix2D *H)
{
  return tmp_fun(pts1,pts2,H);
}

//
//: Compute the 2D affine transformation
//
bool
HMatrix2DAffineCompute::tmp_fun(const PointArray &pts1,
                                 const PointArray &pts2,
                                 HMatrix2D *H)
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

  vnl_double_2x2 Aff = -1.0*(vnl_matrix_inverse<double>(M)*N.as_ref());
  vnl_double_2 t = mn2 - Aff*mn1;

  vnl_double_3x3 T;
  T.set_identity();
  T.update(Aff);
  T(0,2) = t[0];
  T(1,2) = t[1];
  H->set(T);
  return true;
}

//--------------------------------------------------------------------------------

NonHomg::NonHomg(const vcl_vector<HomgPoint2D> &A)
  : vnl_matrix<double>(A.size(),2)
{
  vnl_matrix<double> &X = *this;
  int n = rows();
  for (int i=0; i<n; ++i)
    A[i].get_nonhomogeneous(X(i,0),X(i,1));
}

vnl_double_2 mean2(const vnl_matrix<double> &A) {
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

vnl_matrix<double>& sub_rows(vnl_matrix<double> &A, const vnl_double_2 a) {
  unsigned c = A.columns();
  unsigned r = A.rows();
  assert(c == a.size());
  for (unsigned j=0; j < c; ++j)
    for (unsigned i=0; i < r; ++i)
      A(i,j) -= a[j];

  return A;
}
