#ifndef _HomgMatchPoint3D2D_h
#define _HomgMatchPoint3D2D_h
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//
// .NAME HomgMatchPoint3D2D - A match between a 3D and 2D point
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgMatchPoint3D2D.h
// .FILE HomgMatchPoint3D2D.cxx
//
// .SECTION Description:
// A class to hold a match between a 3D and 2D point.
//

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>

class HomgMatchPoint3D2D {

  // PUBLIC INTERFACE--------------------------------------------------------
public:

  // Constructors/Initializers/Destructors-----------------------------------

  HomgMatchPoint3D2D ();
  HomgMatchPoint3D2D (HomgPoint3D *point3D_ptr, HomgPoint2D *point2D_ptr);
  ~HomgMatchPoint3D2D ();

  // Data Access-------------------------------------------------------------

  HomgPoint3D get_point3D (void);
  HomgPoint2D get_point2D (void);
  void set (HomgPoint3D *point3D_ptr, HomgPoint2D *point2D_ptr);

  // INTERNALS---------------------------------------------------------------
private:
  // Data Members------------------------------------------------------------
    HomgPoint2D _point2D;
    HomgPoint3D _point3D;
};

#endif // _HomgMatchPoint3D2D_h
