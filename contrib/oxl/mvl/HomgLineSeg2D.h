// This is oxl/mvl/HomgLineSeg2D.h
#ifndef HomgLineSeg2D_h_
#define HomgLineSeg2D_h_
//:
// \file
// \brief Homogeneous 2D line segment
//
// A class to hold homogeneous 3-vectors for the two endpoints and
// line for a 2D line segment.
//
// \verbatim
//  Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HomgLineSeg2D : public HomgLine2D
{
  HomgPoint2D point1_;
  HomgPoint2D point2_;
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  HomgLineSeg2D () = default;
  HomgLineSeg2D (const HomgPoint2D& point1, const HomgPoint2D& point2);
  HomgLineSeg2D (double x0, double y0, double x1, double y1);
  ~HomgLineSeg2D () = default;

  // Data Access---------------------------------------------------------------

  // get the line through the two points.
  const HomgLine2D& get_line () const;
  const HomgPoint2D& get_point1 () const;
  const HomgPoint2D& get_point2 () const;

  void set (const HomgPoint2D& point1, const HomgPoint2D& point2);

  double picking_distance(const HomgPoint2D& point1) const;
};

std::ostream& operator<<(std::ostream& s, const HomgLineSeg2D& );

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
  return point1_;
}

//--------------------------------------------------------------
//
//: Return the second point of the line segment
inline const HomgPoint2D& HomgLineSeg2D::get_point2 () const
{
  return point2_;
}

#endif // HomgLineSeg2D_h_
