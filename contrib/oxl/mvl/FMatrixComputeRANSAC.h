#ifndef FMatrixComputeRANSAC_h_
#define FMatrixComputeRANSAC_h_
//:
// \file
// \brief Robust FMatrix computation
//
//    FMatrixComputeRANSAC implements
//    Phil Torr's RANSAC (Robust Sampling Concensus) of the fundamental matrix.
//
//    The resulting F matrix is rank-2 truncated.  The
//     conditioning and truncation are optional and may be
//    omitted. The RANSAC class is also responsible for maintaining the inliers
//    passed in the PairMatchSetCorner class.
//
// \author
//     David N. McKinnon, UQ I.R.I.S., 17.05.2001
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixComputeRobust.h>

class FMatrixComputeRANSAC : public FMatrixComputeRobust {
public:
  //: Initialize FMatrixComputeRobust object.
  // If rank2_truncate = false, the resulting solution is not forced to rank 2
  // using the vnl_svd<double>.
  FMatrixComputeRANSAC(bool rank2_truncate, double std);
  virtual ~FMatrixComputeRANSAC();

  virtual double calculate_term(vcl_vector<double>& residuals,
                                vcl_vector<bool>& inlier_list,
                                int& count);
  virtual double calculate_residual(HomgPoint2D& one,
                                    HomgPoint2D& two,
                                    FMatrix* F);
  virtual double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two,
                                    FMatrix* F);
};

#endif // FMatrixComputeRANSAC_h_
