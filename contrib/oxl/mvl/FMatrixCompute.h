// This is oxl/mvl/FMatrixCompute.h
#ifndef _FMatrixCompute_h
#define _FMatrixCompute_h
//:
// \file
// \brief Base class for fundamental matrix fitters
//
// FMatrixCompute defines the abstract interface to an object which can compute
// a fundamental matrix.  The method compute should be overloaded by
// implementation subclasses.
//
// The input points are supplied in a class PairMatchSetCorner, which may be
// updated by any robust methods to reflect the new set of inliers.
//
// Currently implemented subclasses:
//
// - FMatrixComputeLinear    - Hartley's normalized linear method
// - FMatrixComputeRANSAC    - Oxford robust RANSAC computation
// - FMatrixComputeNonLinear - Oxford nonlinear optimization
// - FMatrixCompute7Point    - Oxford implementation of 7-point algorithm
// - FMatrixCompute8Point    - linear least squares on 8 point matches
// - FMatrixComputeLMedSq    - Zhengyou Zhang's Least Medium of Squares estimation
// - FMatrixComputeMLESAC    - Phil Torr's Maximum Likelihood estimation
// - FMatrixComputeRANSAC    - Phil Torr's Robust Sampling Consensus
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/FMatrix.h>
#include <vgl/vgl_homg_point_2d.h>
class HomgPoint2D;
class PairMatchSetCorner;

class FMatrixCompute
{
 public:
  FMatrixCompute();
  virtual ~FMatrixCompute();

  //: This is the virtual compute interface
  // These 3 functions are implemented in terms of each other,
  // so it suffices to implement exactly one of them in a derived class,
  // and implement the other ones by calling this implementation.
  virtual bool compute(PairMatchSetCorner& matched_points, FMatrix* f_matrix_ptr);
  virtual bool compute(std::vector<HomgPoint2D>&, std::vector<HomgPoint2D>&, FMatrix* f_matrix_ptr);
  virtual bool compute(std::vector<vgl_homg_point_2d<double> >&,
                       std::vector<vgl_homg_point_2d<double> >&,
                       FMatrix& f_matrix_ptr);

  //: Compute fundamental matrix using given matchlist and return an FMatrix object.
  //  This is implemented in terms of compute(MatchList*, FMatrix*)
  inline FMatrix compute(PairMatchSetCorner& matched_points)
    { FMatrix ret; compute(matched_points, &ret); return ret; }

  inline FMatrix compute(std::vector<HomgPoint2D>& pts1, std::vector<HomgPoint2D>& pts2)
    { FMatrix ret; compute(pts1, pts2, &ret); return ret; }
  inline FMatrix compute(std::vector<vgl_homg_point_2d<double> >& pts1,
                         std::vector<vgl_homg_point_2d<double> >& pts2)
    { FMatrix ret; compute(pts1, pts2, ret); return ret; }
};

#endif // _FMatrixCompute_h
