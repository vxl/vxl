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
#ifndef FMSkewComputeLinear_h_
#define FMSkewComputeLinear_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : FMSkewComputeLinear
//
// .SECTION Description
//
//    FMSkewComputeLinear is a subclass of FMatrixCompute, and implements
//    the linear least squares algorithm for estimation of
//    the fundamental matrix which has the skew form of FMatrixSkew.
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95],but the conditioning is optional and may be
//    omitted. The same conditioning is applied to both sets of points so that
//    the skew form of the matrix is maintained.
//
//    Note: As with any nonrobust algorithm, mismatches in the input data
//    may severely effect the result.
//
// .NAME        FMSkewComputeLinear - Linear fit for skew F matrix.
// .LIBRARY MViewCompute
// .HEADER	MultiView Package
// .INCLUDE     FMSkewComputeLinear.h
// .FILE        FMSkewComputeLinear.C
// .SECTION Author
//     Martin Armstrong, Oxford RRG, 20 Nov 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixCompute.h>

class FMatrixSkew;

class FMSkewComputeLinear : public FMatrixCompute {
public:
     // -- Initialize FMSkewComputeLinear object.  If precondition = false,
     // points are not conditioned prior to computation.
     FMSkewComputeLinear(bool precondition = true);
  
     // Computations--------------------------------------------------------------
     // -- Compute a fundamental matrix for arrays of HomgPoint2D.
     // Return false if the calculation fails or there are fewer than eight point
     // matches in the list.
     //
     bool compute(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrixSkew* F);

private:

     bool compute_preconditioned (vcl_vector<HomgPoint2D>& points1,
				  vcl_vector<HomgPoint2D>& points2,
				  FMatrixSkew *F);

     bool _precondition;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS FMSkewComputeLinear.
