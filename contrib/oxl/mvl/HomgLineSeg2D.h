// This is oxl/mvl/HomgLineSeg2D.h
#ifndef _HomgLineSeg2D_h
#define _HomgLineSeg2D_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief Homogeneous 2D line segment
//
// A class to hold homogeneous 3-vectors for the two endpoints and
// line for a 2D line segment.
//
// \verbatim
// Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim

#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <vcl_iosfwd.h>

class HomgLineSeg2D : public HomgLine2D
{
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  HomgLineSeg2D () {}
  HomgLineSeg2D (const HomgPoint2D& point1, const HomgPoint2D& point2);
  HomgLineSeg2D (double x0, double y0, double x1, double y1);
  ~HomgLineSeg2D () {}

  // Data Access---------------------------------------------------------------

  // get the line through the two points.
  const HomgLine2D& get_line () const;
  const HomgPoint2D& get_point1 () const;
  const HomgPoint2D& get_point2 () const;

  void set (const HomgPoint2D& point1, const HomgPoint2D& point2);

  double picking_distance(const HomgPoint2D& point1) const;

 private:
  HomgPoint2D _point1;
  HomgPoint2D _point2;
};

vcl_ostream& operator<<(vcl_ostream& s, const HomgLineSeg2D& );

//--------------------------------------------------------------
//
//: Return the line through the two points.
inline const HomgLine2D& HomgLineSeg2D::get_line () const
{
  return *this;
}

//--------------------------------------------------------------
//
//: Return the first point of the line segment
inline const HomgPoint2D& HomgLineSeg2D::get_point1 () const
{
  return _point1;
}

//--------------------------------------------------------------
//
//: Return the second point of the line segment
inline const HomgPoint2D& HomgLineSeg2D::get_point2 () const
{
  return _point2;
}

#endif // _HomgLineSeg2D_h
