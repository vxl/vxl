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
#pragma implementation
#endif
//--------------------------------------------------------------
//
// Class : HomgLineSeg3D
//
// Modifications : see HomgLineSeg3D.h
//
//--------------------------------------------------------------

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_cmath.h>
#include <vcl/vcl_memory.h>
#include <vcl/vcl_cstdlib.h>
#include <mvl/HomgLine3D.h>
#include <mvl/HomgLineSeg3D.h>
#include <mvl/HomgOperator3D.h>


//--------------------------------------------------------------
//
// -- Default constructor
HomgLineSeg3D::HomgLineSeg3D ()
{
}

//--------------------------------------------------------------
//
// -- Constructor forming line segment from start and end points
HomgLineSeg3D::HomgLineSeg3D (const HomgPoint3D& point1, const HomgPoint3D& point2):
  HomgLine3D(point1, point2)
{
  _point1 = point1;
  _point2 = point2;
}


//--------------------------------------------------------------
//
// -- Destructor
HomgLineSeg3D::~HomgLineSeg3D()
{
}


//--------------------------------------------------------------
//
// -- Return the line through the two points.
const HomgLine3D& HomgLineSeg3D::get_line () const
{
  return *this;
}

//--------------------------------------------------------------
//
// -- Return the first point of the line segment
const HomgPoint3D& HomgLineSeg3D::get_point1 () const
{
  return _point1;
}

//--------------------------------------------------------------
//
// -- Return the second point of the line segment
const HomgPoint3D& HomgLineSeg3D::get_point2 () const
{
  return _point2;
}


//--------------------------------------------------------------
//
// Set the line segment given two points
void HomgLineSeg3D::set (const HomgPoint3D& point1, const HomgPoint3D& point2) 
{
  _point1 = point1;
  _point2 = point2;
}

ostream& operator << (ostream& s, const HomgLineSeg3D& l)
{
  return s << l.get_point1().get_vector() << ", " << l.get_point2().get_vector();
}
