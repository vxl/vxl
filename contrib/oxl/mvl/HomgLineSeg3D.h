#ifndef _HomgLineSeg3D_h
#define _HomgLineSeg3D_h
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//
// .NAME HomgLineSeg3D - Homogeneous 3D line segment
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgLineSeg3D.h
// .FILE HomgLineSeg3D.cxx
//
// .SECTION Description:
// A class to hold a homogeneous representation of a 3D linesegment
//
// .SECTION Modifications:
//   Peter Vanroose - 11 Mar 97 - added operator==
//

#include <mvl/HomgLine3D.h>
#include <mvl/HomgOperator3D.h>
#include <mvl/HomgPoint3D.h>
#include <vcl_iosfwd.h>

class HomgLineSeg3D : public HomgLine3D {

  // PUBLIC INTERFACE----------------------------------------------------------
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

  // INTERNALS-----------------------------------------------------------------
private:
  // Data Members--------------------------------------------------------------
  HomgPoint3D _point1;
  HomgPoint3D _point2;
};

ostream& operator<<(ostream& s, const HomgLineSeg3D& );

#endif // _HomgLineSeg3D_h
