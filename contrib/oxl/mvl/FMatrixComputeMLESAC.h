#ifndef FMatrixComputeMLESAC_h_
#define FMatrixComputeMLESAC_h_
// .NAME    FMatrixComputeMLESAC - Robust FMatrix computation
// .LIBRARY MViewCompute
// .HEADER  MultiView Package
// .INCLUDE mvl/FMatrixComputeMLESAC.h
// .FILE    FMatrixComputeMLESAC.cxx
// .EXAMPLE 
//
// .SECTION Description
//    FMatrixComputeMLESAC implements
//    Phil Torr's MLESAC (Maximum Likelyhood Estimation) of the fundamental matrix.    
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95], and the resulting F matrix is rank-2
//    truncated.  The conditioning and truncation are optional and may be
//    omitted.
//
// .SECTION Author
//     David N. McKinnon, UQ I.R.I.S., 02.12.2000
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixComputeRobust.h>
#include <mvl/FManifoldProject.h>
#include <vcl/vcl_vector.h>

class FMatrixComputeMLESAC : public FMatrixComputeRobust {
public:
  // -- Initialize FMatrixComputeRobust object.
  // If rank2_truncate = false, the resulting solution is not forced to rank 2
  // using the vnl_svd<double>.
  FMatrixComputeMLESAC(bool rank2_truncate, double std);
  virtual ~FMatrixComputeMLESAC();

  virtual double calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count);
  virtual double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, FMatrix* F);
};

#endif // FMatrixComputeMLESAC_h_
