// This is oxl/mvl/HMatrix2DEuclideanCompute.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "HMatrix2DEuclideanCompute.h"

#include <vcl_cassert.h>

#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_svd.h>

#include <mvl/HMatrix2D.h>
#include <mvl/PairMatchSetCorner.h>
#include <mvl/HMatrix2DAffineCompute.h>

//
//
//
HMatrix2DEuclideanCompute::HMatrix2DEuclideanCompute(void) : HMatrix2DCompute() { }

HMatrix2DEuclideanCompute::~HMatrix2DEuclideanCompute() { }

//
//
//
HMatrix2D
HMatrix2DEuclideanCompute::compute(PairMatchSetCorner const& matches)
{
 PointArray pts1(matches.count());
 PointArray pts2(matches.count());
 matches.extract_matches(pts1, pts2);
 HMatrix2D H;
 tmp_fun(pts1,pts2,&H);
 return H;
}

HMatrix2D
HMatrix2DEuclideanCompute::compute(PointArray const& p1,
                                   PointArray const& p2)
{
  HMatrix2D H;
  tmp_fun(p1,p2,&H);
  return H;
}

bool
HMatrix2DEuclideanCompute::compute_p(PointArray const& pts1,
                                     PointArray const& pts2,
                                     HMatrix2D *H)
{
  return tmp_fun(pts1,pts2,H);
}

bool
HMatrix2DEuclideanCompute::tmp_fun(PointArray const& pts1,
                                   PointArray const& pts2,
                                   HMatrix2D *H)
{
  assert(pts1.size() == pts2.size());

  NonHomg p1(pts1);
  NonHomg p2(pts2);
  vnl_double_2 mn1 = ::mean2(p1);
  vnl_double_2 mn2 = ::mean2(p2);
  sub_rows(p1,mn1);
  sub_rows(p2,mn2);

  vnl_double_2x2 scatter = vnl_transpose(p2)*p1;
  vnl_svd<double> svd(scatter);

  vnl_double_2x2 R = svd.U() * vnl_transpose(svd.V());
  vnl_double_2 t = mn2 - R * mn1;

  vnl_double_3x3 T;
  T.set_identity();
  T.update(R);
  T(0,2) = t[0];
  T(1,2) = t[1];
  H->set(T);
  return true;
}
