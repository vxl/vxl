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
// Class : HomgLineSeg2D
//
// Modifications : see HomgLineSeg2D.h
//
//--------------------------------------------------------------

#include <vcl/vcl_iostream.h>

#include <mvl/HomgLine2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <mvl/HomgOperator2D.h>
////////////////#include <Geometry/ImplicitLine.h>

//--------------------------------------------------------------
//
// -- Constructor forming linesegment from start and end points
HomgLineSeg2D::HomgLineSeg2D (const HomgPoint2D& point1, 
			      const HomgPoint2D& point2):
  HomgLine2D(HomgOperator2D::join (point1, point2)),
  _point1(point1),
  _point2(point2)
{
}

//--------------------------------------------------------------
//
// -- Constructor forming linesegment (x0,y0) -> (x1,y1)
HomgLineSeg2D::HomgLineSeg2D (double x0, double y0, double x1, double y1):
  HomgLine2D(y0 - y1, x1 - x0, x0 * y1 - x1 * y0),
  _point1(HomgPoint2D(x0, y0, 1.0)),
  _point2(HomgPoint2D(x1, y1, 1.0))
{
}

//--------------------------------------------------------------
//
// -- Set the line segment given two points
void HomgLineSeg2D::set (const HomgPoint2D& point1, 
			 const HomgPoint2D& point2) 
{
  _point1 = point1;
  _point2 = point2;
  HomgLine2D::operator=(HomgOperator2D::join (point1, point2));
}

//-----------------------------------------------------------------------------
//
// -- print to ostream
ostream& operator<<(ostream& s, const HomgLineSeg2D& p)
{
  return s <<
    "<HomgLineSeg2D " << p.get_vector() <<
    " from " << p.get_point1() <<
    " to " << p.get_point2() <<
    ">";
}

#ifdef VXL_UNDEF
//-----------------------------------------------------------------------------
//
// -- Convert the segment to an ImplicitLine, allocated using new.
// Return 0 if either endpoint is at infinity.
ImplicitLine* HomgLineSeg2D::get_implicitline() const
{ 
  IUPoint *startpoint = get_point1().get_iupoint();
  IUPoint *endpoint = get_point2().get_iupoint();
  if (!startpoint || !endpoint) {
    cerr << "HomgLineSeg2D::get_implicitline() -- Endpoint at infinity\n";
    cerr << "    " << *this << endl;
    return 0;
  }
  
  return new ImplicitLine(startpoint, endpoint);
}
#endif

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <vnl/vnl_double_2.h>

// -- Return distance to nearest point contained in lineseg
double HomgLineSeg2D::picking_distance(const HomgPoint2D& hp) const
{
  const HomgLineSeg2D& l = *this;

  vnl_double_2 p;
  hp.get_nonhomogeneous(p[0], p[1]);

  vnl_double_2 l1;
  l.get_point1().get_nonhomogeneous(l1[0], l1[1]);
 
  double x1, y1;
  l.get_point2().get_nonhomogeneous(x1, y1);
  vnl_double_2 l2(x1, y1);

  vnl_double_2 dir = l2 - l1;

  double seg_length = dir.magnitude() / 2;
  dir.normalize();

  vnl_double_2 mid = (l2 + l1) / 2;

  vnl_double_2 diff = p - mid;
      
  dir *= dot_product(diff, dir);

  float projection_length = dir.magnitude();
  diff = diff - dir;
  double d = diff.magnitude();
      
  //check if outside
  if (projection_length > seg_length)
    return 1e20;

  return d;
}
