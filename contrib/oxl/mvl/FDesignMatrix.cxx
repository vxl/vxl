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
#ifdef __GNUC__
#pragma implementation "FDesignMatrix.h"
#endif
//
// Class: FDesignMatrix
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 21 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "FDesignMatrix.h"

// -- Construct a design matrix for the fundamental matrix least
// squares problem. Specifically, it constructs the matrix
// @{\[
// D = \mmatrix{9}{ m_{11} m'_{11} & m_{12} m_{11}'  & m_{13} m'_{11} &
//          m_{11} m'_{12} & m_{12} m'_{12}  & m_{13} m'_{12} &
//          m_{11} m'_{13} & m_{12} m'_{13}  & m_{13} m'_{13} \cr
//          \vdots\cr
//          m_{n1} m'_{n1} & m_{n2} m'_{n1}  & m_{n3} m'_{n1} &
//	    m_{n1} m'_{n2} & m_{n2} m'_{n2}  & m_{n3} m'_{n2} &
// 	    m_{n1} m'_{n3} & m_{n2} m'_{n3}  & m_{n3} m'_{n3} }
// \]
// such that $ D f = 0 $ for
// $f = (F_{11} F_{12} F_{13} F_{21} F_{22} F_{23} F_{31} F_{32} F_{33}) $
// where $F$ is the fundamental matrix.
// @} 
//

FDesignMatrix::FDesignMatrix(vcl_vector<HomgPoint2D>& points1,
			     vcl_vector<HomgPoint2D>& points2):
  base(points1.size(), 9)
{
  int n = points1.size();
  for(int row = 0; row < n; ++row) {
    const HomgPoint2D& p1 = points1[row];
    const HomgPoint2D& p2 = points2[row];

    vnl_matrix<double>& D = *this;
    D(row,0) = p1.get_x() * p2.get_x();
    D(row,1) = p1.get_y() * p2.get_x();
    D(row,2) = p1.get_w() * p2.get_x();
    D(row,3) = p1.get_x() * p2.get_y();
    D(row,4) = p1.get_y() * p2.get_y();
    D(row,5) = p1.get_w() * p2.get_y();
    D(row,6) = p1.get_x() * p2.get_w();
    D(row,7) = p1.get_y() * p2.get_w();
    D(row,8) = p1.get_w() * p2.get_w();
  }
}
