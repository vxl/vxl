// This is oxl/mvl/HomgLineSeg2D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "HomgLineSeg2D.h"
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgOperator2D.h>
#include <vnl/vnl_double_2.h>
#include <vcl_iostream.h>

//--------------------------------------------------------------
//
//: Constructor forming linesegment from start and end points
HomgLineSeg2D::HomgLineSeg2D (const HomgPoint2D& point1,
                              const HomgPoint2D& point2):
  HomgLine2D(HomgOperator2D::join (point1, point2)),
  _point1(point1),
  _point2(point2)
{
}

//--------------------------------------------------------------
//
//: Constructor forming linesegment (x0,y0) -> (x1,y1)
HomgLineSeg2D::HomgLineSeg2D (double x0, double y0, double x1, double y1):
  HomgLine2D(y0 - y1, x1 - x0, x0 * y1 - x1 * y0),
  _point1(HomgPoint2D(x0, y0, 1.0)),
  _point2(HomgPoint2D(x1, y1, 1.0))
{
}

//--------------------------------------------------------------
//
//: Set the line segment given two points
void HomgLineSeg2D::set (const HomgPoint2D& point1,
                         const HomgPoint2D& point2)
{
  _point1 = point1;
  _point2 = point2;
  HomgLine2D::operator=(HomgOperator2D::join (point1, point2));
}

//-----------------------------------------------------------------------------
//
//: print to vcl_ostream
vcl_ostream& operator<<(vcl_ostream& s, const HomgLineSeg2D& p)
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
//: Convert the segment to an ImplicitLine, allocated using new.
// Return 0 if either endpoint is at infinity.
ImplicitLine* HomgLineSeg2D::get_implicitline() const
{
  IUPoint *startpoint = get_point1().get_iupoint();
  IUPoint *endpoint = get_point2().get_iupoint();
  if (!startpoint || !endpoint) {
    vcl_cerr << "HomgLineSeg2D::get_implicitline() -- Endpoint at infinity\n";
    vcl_cerr << "    " << *this << vcl_endl;
    return 0;
  }

  return new ImplicitLine(startpoint, endpoint);
}
#endif

//: Return distance to nearest point contained in lineseg
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

  vnl_double_2 mid = (l2 + l1) / 2.0;

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
