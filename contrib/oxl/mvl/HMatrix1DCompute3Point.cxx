// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

//--------------------------------------------------------------
//
// Class : HMatrix1DCompute3Point
//
// Modifications : see HMatrix1DCompute3Point.h
//
//--------------------------------------------------------------

#include "HMatrix1DCompute3Point.h"

#include <vcl/vcl_vector.h>
#include <mvl/HMatrix1D.h>

//
//	computes 1d Moebius map from three point correspondences :
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
HMatrix1DCompute3Point::~HMatrix1DCompute3Point() { }

bool
HMatrix1DCompute3Point::compute_cool_homg(const vcl_vector<HomgPoint1D>& points1,
					  const vcl_vector<HomgPoint1D>& points2,
					  HMatrix1D *H)
{
  assert(points1.size() == 3);
  assert(points2.size() == 3);
  double T[2][2];
  direct_compute(T,
		 points1[0].get_x() , points1[1].get_x() , points1[2].get_x(),
		 points1[0].get_w() , points1[1].get_w() , points1[2].get_w(),
		 
		 points2[0].get_x() , points2[1].get_x() , points2[2].get_x(),
		 points2[0].get_w() , points2[1].get_w() , points2[2].get_w());
  H->set((double*)T);// ugly cast. FIXME
  return true;
}

