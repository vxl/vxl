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
#ifdef __GNUG__
#pragma implementation "FMatrixAffine.h"
#endif
//--------------------------------------------------------------
//
// Class : FMatrixAffine
//
// Modifications : see FMatrixAffine.h
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>
//#include <vcl/vcl_memory.h>
#include <vcl/vcl_cstdlib.h>
#include <vnl/vnl_matrix.h>
#include <mvl/FMatrixAffine.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PMatrix.h>

//--------------------------------------------------------------
//
// -- Constructor
FMatrixAffine::FMatrixAffine ()
{
}

//--------------------------------------------------------------
//
// -- Destructor
FMatrixAffine::~FMatrixAffine()
{
}

//--------------------------------------------------------------
// 
// -- Set the fundamental matrix using the two-dimensional (C-storage)
// array f_matrix. Only returns true if f_matrix contained a Fundamental
// matrix in the affine form, and not an approximation to one.
// Otherwise returns false and the matrix is not set.
// f_matrix must be 3x3, i.e., must contain 9 elements.

bool FMatrixAffine::set (const double* f_matrix)
{
  for (int row_index = 0; row_index < 3; row_index++)
  for (int col_index = 0; col_index < 3; col_index++)
  {
    _f_matrix. put (row_index, col_index, *f_matrix);
    _ft_matrix. put (col_index, row_index, *f_matrix++);
  }

  return true;
}
