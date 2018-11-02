#include <iostream>
#include <vector>
#include "HMatrix1DCompute3Point.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_homg_point_1d.h>
#include <mvl/HMatrix1D.h>

//
// computes 1d Moebius map from three point correspondences :
//
static
void
direct_compute(double T[2][2],
               double p11,double p12,double p13,// p1 p2 p3
               double p21,double p22,double p23,

               double q11,double q12,double q13,// q1 q2 q3
               double q21,double q22,double q23)
{
  double A[2][2],B[2][2];
  double t;

  t=+(p22*p13-p12*p23); A[0][0]=t*p11; A[1][0]=t*p21;
  t=-(p21*p13-p11*p23); A[0][1]=t*p12; A[1][1]=t*p22;

  t=+(q22*q13-q12*q23); B[0][0]=t*q11; B[1][0]=t*q21;
  t=-(q21*q13-q11*q23); B[0][1]=t*q12; B[1][1]=t*q22;

  T[0][0]= B[0][0]*A[1][1]-B[0][1]*A[1][0];
  T[1][0]= B[1][0]*A[1][1]-B[1][1]*A[1][0];
  T[0][1]=-B[0][0]*A[0][1]+B[0][1]*A[0][0];
  T[1][1]=-B[1][0]*A[0][1]+B[1][1]*A[0][0];
}

HMatrix1DCompute3Point::HMatrix1DCompute3Point(void) : HMatrix1DCompute() { }
HMatrix1DCompute3Point::~HMatrix1DCompute3Point() = default;

bool
HMatrix1DCompute3Point::compute_cool_homg(const std::vector<vgl_homg_point_1d<double> >& points1,
                                          const std::vector<vgl_homg_point_1d<double> >& points2,
                                          HMatrix1D *H)
{
  assert(points1.size() == 3);
  assert(points2.size() == 3);
  double T[2][2];
  direct_compute(T,
                 points1[0].x() , points1[1].x() , points1[2].x(),
                 points1[0].w() , points1[1].w() , points1[2].w(),

                 points2[0].x() , points2[1].x() , points2[2].x(),
                 points2[0].w() , points2[1].w() , points2[2].w());
  H->set(&T[0][0]);
  return true;
}
