#ifndef FMatrixComputeMLESAC_h_
#define FMatrixComputeMLESAC_h_
//:
// \file
// \brief Robust FMatrix computation
//
//  FMatrixComputeMLESAC implements
//  Phil Torr's MLESAC (Maximum Likelihood Estimation) of the fundamental matrix.
//
//  Points are preconditioned as described in [Hartley, ``In defense of
//  the 8-point algorithm'', ICCV95], and the resulting F matrix is rank-2
//  truncated.  The conditioning and truncation are optional and may be
//  omitted.
//
// \author
//     David N. McKinnon, UQ I.R.I.S., 02.12.2000
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
#include <mvl/FMatrixComputeRobust.h>
#include <mvl/FManifoldProject.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class FMatrixComputeMLESAC : public FMatrixComputeRobust
{
 public:
  //: Initialize FMatrixComputeRobust object.
  // If rank2_truncate = false, the resulting solution is not forced to rank 2
  // using the vnl_svd<double>.
  FMatrixComputeMLESAC(bool rank2_truncate, double std);
  ~FMatrixComputeMLESAC() override;

  double calculate_term(std::vector<double>& residuals, std::vector<bool>& inlier_list, int& count) override;
  double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F) override;
  double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two,
                                    FMatrix* F) override;
};

#endif // FMatrixComputeMLESAC_h_
