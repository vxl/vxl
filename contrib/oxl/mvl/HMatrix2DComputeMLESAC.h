#ifndef HMatrix2DComputeMLESAC_h_
#define HMatrix2DComputeMLESAC_h_
//:
// \file
// \brief Robust HMatrix computation
//
//  HMatrix2DComputeMLESAC implements
//  Phil Torr's MLESAC (Maximum Likelihood Estimation) of the projective matrix.
//  (or homography)
//
//  Points are preconditioned as described in [Hartley, ``In defense of
//  the 8-point algorithm'', ICCV95].
//    The conditioning and truncation are optional and may be
//  omitted. The MLESAC class is also responsible for maintaining the inliers
//  passed in the PairMatchSetCorner class.
//
// \author
//     David N. McKinnon, UQ I.R.I.S., 23.01.2001
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <vgl/vgl_fwd.h>
#include <mvl/HMatrix2DComputeRobust.h>
#include <mvl/HMatrix2DCompute4Point.h>
#include <mvl/HMatrix2D.h>
#include <mvl/AffineMetric.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HMatrix2DComputeMLESAC : public HMatrix2DComputeRobust
{
 public:
  //: Initialize HMatrix2DComputeRobust object.
  HMatrix2DComputeMLESAC(double std);
  ~HMatrix2DComputeMLESAC() override;

  double calculate_term(std::vector<double>& residuals,
                                std::vector<bool>& inlier_list,
                                int& count) override;
  double calculate_residual(HomgPoint2D& one,
                                    HomgPoint2D& two,
                                    HMatrix2D* F) override;
  double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two,
                                    HMatrix2D* F) override;
};

#endif // HMatrix2DComputeMLESAC_h_
