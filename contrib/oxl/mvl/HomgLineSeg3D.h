// This is oxl/mvl/HomgLineSeg3D.h
#ifndef HomgLineSeg3D_h_
#define HomgLineSeg3D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include <mvl/HomgLine3D.h>
#include <mvl/HomgOperator3D.h>
#include <mvl/HomgPoint3D.h>
#include <vcl_iosfwd.h>

class HomgLineSeg3D : public HomgLine3D
{
  // Data Members--------------------------------------------------------------
  HomgPoint3D _point1;
  HomgPoint3D _point2;

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

vcl_ostream& operator<<(vcl_ostream& s, const HomgLineSeg3D& );

#endif // HomgLineSeg3D_h_
