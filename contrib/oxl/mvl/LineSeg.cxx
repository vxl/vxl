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
#pragma implementation
#endif

//-----------------------------------------------------------------------------
//
// Class: LineSeg
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 01 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include "LineSeg.h"

// -- Constructor
LineSeg::LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean)
{
  _x0 = x0;
  _y0 = y0;
  _x1 = x1;
  _y1 = y1;
  _theta = theta;
  _grad_mean = grad_mean;  
}

// -- Save to ostream
ostream& operator<<(ostream& s, const LineSeg& l)
{
  return s << l._x0 << " "
	   << l._y0 << " "
	   << l._x1 << " "
	   << l._y1 << " "
	   << l._theta << " "
	   << l._grad_mean << endl;
}

// -- Read from istream
istream& operator>>(istream& s, LineSeg& l)
{
  return s >> l._x0 >> l._y0 >> l._x1 >> l._y1 >> l._theta >> l._grad_mean;
}
