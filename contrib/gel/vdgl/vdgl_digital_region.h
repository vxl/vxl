#ifndef vdgl_digital_region_h
#define vdgl_digital_region_h
#ifdef __GNUC__
#pragma interface
#endif

// .NAME vdgl_digital_region - Represents a 2D digital_region
// .INCLUDE vgl/vdgl_digital_region.h
// .FILE vdgl_digital_region.txx
//
// .SECTION Description
//  A 2d image digital_region
//
// .SECTION Author
//    Geoff Cross
// Created: xxx xx xxxx

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>

class vdgl_digital_region {
   // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Destructors--------------------------------------------------
  vdgl_digital_region();
  ~vdgl_digital_region();

  // Operators----------------------------------------------------------------

  bool add_point( const vgl_point_2d<double> &p);
  friend vcl_ostream& operator<<(vcl_ostream& s, const vdgl_digital_region& d);

  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------

  vcl_vector< vgl_point_2d<double> > ps_;
};

#endif // _vdgl_digital_region_h
