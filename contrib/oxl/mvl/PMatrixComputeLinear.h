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
//--------------------------------------------------------------
//
// Class : PMatrixComputeLinear
//
// .SECTION Description:
//
// @{ PMatrixComputeLinear contains a linear method to calculate the P matrix 
// from at least six 3D-to-2D point correspondences.  Precisely, given
// 3D points $\vec X_i$ and their images $\vec u_i$, we compute $P$ such that
// $P \vec X_i \sim \vec u_i$. @}
//
// .NAME PMatrixComputeLinear
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/PMatrixComputeLinear.h
// .FILE PMatrixComputeLinear.cxx
//

#ifndef _PMatrixComputeLinear_h
#define _PMatrixComputeLinear_h

#include <vcl/vcl_list.h>
#include <mvl/PMatrixCompute.h>

class PMatrixComputeLinear : public PMatrixCompute {
public:
  bool compute (PMatrix *t_matrix_ptr);
  bool compute (vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint3D>& points2, PMatrix *P);
};
#endif
