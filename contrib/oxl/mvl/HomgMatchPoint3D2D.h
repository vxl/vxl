// This is oxl/mvl/HomgMatchPoint3D2D.h
#ifndef HomgMatchPoint3D2D_h_
#define HomgMatchPoint3D2D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A match between a 3D and 2D point
//
// A class to hold a match between a 3D and 2D point.
//

#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>

class HomgMatchPoint3D2D
{
  // Data Members------------------------------------------------------------
    HomgPoint2D _point2D;
    HomgPoint3D _point3D;

 public:
  // Constructors/Initializers/Destructors-----------------------------------

  HomgMatchPoint3D2D ();
  HomgMatchPoint3D2D (HomgPoint3D *point3D_ptr, HomgPoint2D *point2D_ptr);
  ~HomgMatchPoint3D2D ();

  // Data Access-------------------------------------------------------------

  HomgPoint3D get_point3D (void);
  HomgPoint2D get_point2D (void);
  void set (HomgPoint3D *point3D_ptr, HomgPoint2D *point2D_ptr);
};

#endif // HomgMatchPoint3D2D_h_
