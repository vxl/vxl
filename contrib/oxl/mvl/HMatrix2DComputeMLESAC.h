#ifndef HMatrixC2DomputeMLESAC_h_
#define HMatrix2DComputeMLESAC_h_
// .NAME    HMatrix2DComputeMLESAC - Robust FMatrix computation
// .LIBRARY MViewCompute
// .HEADER  MultiView Package
// .INCLUDE mvl/HMatrix2DComputeMLESAC.h
// .FILE    HMatrix2DComputeMLESAC.cxx
// .EXAMPLE 
//
// .SECTION Description
//    HMatrix2DComputeMLESAC implements
//    Phil Torr's MLESAC (Maximum Likelyhood Estimation) of the projective matrix.    
//    (or homography)
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95].
//      The conditioning and truncation are optional and may be
//    omitted. The MLESAC class is also responsible for maintaining the inliers
//    passed in the PairMatchSetCorner class.
//
// .SECTION Author
//     David N. McKinnon, UQ I.R.I.S., 23.01.2001
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <mvl/HMatrix2DComputeRobust.h>
#include <mvl/HMatrix2DCompute4Point.h>
#include <mvl/HMatrix2D.h>
#include <mvl/AffineMetric.h>
#include <vcl/vcl_vector.h>

class HMatrix2DComputeMLESAC : public HMatrix2DComputeRobust {
public:
  // -- Initialize HMatrix2DComputeRobust object.
  HMatrix2DComputeMLESAC(double std);
  virtual ~HMatrix2DComputeMLESAC();

  virtual double calculate_term(vcl_vector<double>& residuals, vcl_vector<bool>& inlier_list, int& count);
  virtual double calculate_residual(HomgPoint2D& one, HomgPoint2D& two, HMatrix2D* F);

};
#endif // HMatrix2DComputeMLESAC_h_
