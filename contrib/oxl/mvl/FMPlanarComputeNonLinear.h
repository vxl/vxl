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
//-*- c++ -*-------------------------------------------------------------------
#ifndef FMPlanarComputeNonLinear_h_
#define FMPlanarComputeNonLinear_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .SECTION Description
// 
//    FMPlanarComputeNonLinear fits a planar fundamental matrix to point matches
//    by minimizing the Luong-Faugeras [ECCV '92] error function
//    @{\[
//    E = \sum_{i=1}^n d^2({\bf x}'_i, {\tt F} {\bf x}_i) + d^2({\bf x}_i, {\tt F}^\top {\bf x}'_i)
//    \]@}
//    Minimization currently uses vnl_levenberg_marquardt with finite-difference
//    derivatives, and does not minimize a Huber function---all matches
//    are assumed to be inliers.

//
// .NAME        FMPlanarComputeNonLinear - Nonlinear least squares planar fundamental matrix fit.
// .LIBRARY     MViewCompute
// .HEADER	MultiView Package
// .INCLUDE     mvl/FMPlanarComputeNonLinear.h
// .FILE        FMPlanarComputeNonLinear.cxx
// .SECTION Author
//     Martin Armstrong, Oxford 21/11/96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <mvl/HomgMetric.h>
#include <mvl/FMatrixCompute.h>

class ImageMetric;
class PairMatchSetCorner;
class FMatrixPlanar;

class FMPlanarComputeNonLinear : public FMatrixCompute {
public:
  
  // Constructors/Destructors--------------------------------------------------
  FMPlanarComputeNonLinear(const ImageMetric*, const ImageMetric*, double outlier_threshold = 0);
  
  // Operations----------------------------------------------------------------
  
  // Computations--------------------------------------------------------------

  bool compute_planar(PairMatchSetCorner& matches, FMatrixPlanar* F);
  bool compute_planar(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2, FMatrixPlanar* F);

  // FMatrixCompute virtuals
  bool compute(PairMatchSetCorner& matches, FMatrix* F);
  bool compute(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2, FMatrix* F);

protected:
  // Data Members--------------------------------------------------------------
  double _outlier_distance_squared;
  
  const ImageMetric* _image_metric1;
  const ImageMetric* _image_metric2;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS FMPlanarComputeNonLinear.
