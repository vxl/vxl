#ifndef FMatrixComputeLMedSq_h_
#define FMatrixComputeLMedSq_h_
//:
// \file
// \brief Robust FMatrix computation
//
//    FMatrixComputeLMedSq implements
//    Zhengyou Zhang's LMedSq (Least Medium of Squares Estimation) of the fundamental matrix.
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95], and the resulting F matrix is rank-2
//    truncated.  The conditioning and truncation are optional and may be
//    omitted.
//
// \author
//     David N. McKinnon, UQ I.R.I.S., 02.12.2000
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixComputeRobust.h>
#include <vcl_vector.h>

class FMatrixComputeLMedSq : public FMatrixComputeRobust {
public :
  //: Initialize FMatrixComputeRobust object.
  // If rank2_truncate = false, the resulting solution is not forced to rank 2
  // using the vnl_svd<double>.
  FMatrixComputeLMedSq(bool rank2_truncate, int size);
  virtual ~FMatrixComputeLMedSq();

  virtual double calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count);
  virtual double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F);
  virtual double calculate_residual(vgl_homg_point_2d<double>& one,
                                    vgl_homg_point_2d<double>& two, FMatrix* F);
private :
  // Helper Functions
  // Return the median vaule for a vector of residuals
  double median(vcl_vector<double> residuals);
};

#endif // FMatrixComputeLMedSq_h_
