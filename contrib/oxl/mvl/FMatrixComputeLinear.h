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
#ifndef FMatrixComputeLinear_h_
#define FMatrixComputeLinear_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : FMatrixComputeLinear
//
// .SECTION Description
//    FMatrixComputeLinear is a subclass of FMatrixCompute, and implements
//    the ``8-point'' or linear least squares algorithm for estimation of
//    the fundamental matrix.
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95], and the resulting F matrix is rank-2
//    truncated.  The conditioning and truncation are optional and may be
//    omitted.
//
//    Note: As with any nonrobust algorithm, mismatches in the input data
//    may severely effect the result.
//
// .NAME    FMatrixComputeLinear - Hartley 8-point fundamental matrix fit.
// .LIBRARY MViewCompute
// .HEADER  MultiView Package
// .INCLUDE mvl/FMatrixComputeLinear.h
// .FILE    FMatrixComputeLinear.cxx
// .EXAMPLE ../examples/exampleFMatrixCompute.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 21 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixCompute.h>
#include <mvl/FMatrix.h>

class FMatrixComputeLinear : public FMatrixCompute {
public:
// -- Initialize FMatrixComputeLinear object.  If precondition = false,
// points are not conditioned prior to computation.
// If rank2_truncate = false, the resulting solution is not forced to rank 2
// using the vnl_svd<double>.
  FMatrixComputeLinear(bool precondition = true, bool rank2_truncate = true);
  
  // Computations--------------------------------------------------------------
// -- Compute a fundamental matrix for a set of point matches.
// 
// Return false if the calculation fails or there are fewer than eight point
// matches in the list.
//
  bool compute(PairMatchSetCorner&, FMatrix* F);

// -- Interface to above using arrays of HomgPoint2D.  Makes a PairMatchSetCorner,
// and then calls the compute method above.
  bool compute(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrix* F);

// - Interface to above using preconditioned points
  bool compute_preconditioned(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrix* F);

// -- Return an FMatrix computed as above.
  FMatrix compute(PairMatchSetCorner& matches) { return FMatrixCompute::compute(matches); }
  FMatrix compute(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2) {
    return FMatrixCompute::compute(points1, points2);
  }

private:
  bool _precondition;
  bool _rank2_truncate;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS FMatrixComputeLinear.
