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
#ifdef __GNUC__
#pragma implementation "vnl_int_matrix.h"
#endif
//
// Class: vnl_int_matrix
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 27 Dec 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_int_matrix.h"

#include <vcl/vcl_fstream.h>
#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_cctype.h>

// -- Construct from matrix of double.  The double-to-int conversion
// is simply the standard (int) cast.
vnl_int_matrix::vnl_int_matrix(const vnl_matrix<double>& d):
  Base(d.rows(), d.columns())
{
  unsigned m = d.rows();
  unsigned n = d.columns();
  
  for (unsigned i = 0; i < m; ++i)
    for (unsigned j = 0; j < n; ++j)
      (*this)(i,j) = (int)d(i,j);
}

// -- Load from disk
vnl_int_matrix::vnl_int_matrix(char const* filename)
{
  ifstream s(filename);
  read_ascii(s);
}
