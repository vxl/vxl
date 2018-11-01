// This is oxl/mvl/HomgLineSeg3D.h
#ifndef HomgLineSeg3D_h_
#define HomgLineSeg3D_h_
//:
// \file
// \brief Homogeneous 3D line segment
//
// A class to hold a homogeneous representation of a 3D linesegment
//
// \verbatim
// Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <mvl/HomgLine3D.h>
#include <mvl/HomgOperator3D.h>
#include <mvl/HomgPoint3D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class HomgLineSeg3D : public HomgLine3D
{
  // Data Members--------------------------------------------------------------
  HomgPoint3D point1_;
  HomgPoint3D point2_;

 public:
  // Constructors/Initializers/Destructors-------------------------------------

  HomgLineSeg3D ();
  HomgLineSeg3D (const HomgPoint3D &point1, const HomgPoint3D& point2);
  ~HomgLineSeg3D ();

  // Data Access---------------------------------------------------------------

  // get the line through the two points.
  const HomgLine3D& get_line () const;
  const HomgPoint3D& get_point1 () const;
  const HomgPoint3D& get_point2 () const;

  void set (const HomgPoint3D& point1, const HomgPoint3D& point2);
};

std::ostream& operator<<(std::ostream& s, const HomgLineSeg3D& );

#endif // HomgLineSeg3D_h_
