// This is core/vgui/vgui_soview3D.h
#ifndef vgui_soview3D_h_
#define vgui_soview3D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Mar 99
// \brief  3-dimensional spatial object view.
//
// Contains classes:  vgui_soview3D  vgui_point3D  vgui_linseg3D
// Notes:  We use floats instead of doubles as size is a speed issue (sic.)
//

#include "vgui_soview.h"
#include <vcl_iosfwd.h>

//: 3-dimensional spatial object view.
class vgui_soview3D : public vgui_soview
{
 public:
  vgui_soview3D();
  virtual ~vgui_soview3D() {}
};


//: 3-dimensional point.
class vgui_point3D : public vgui_soview3D
{
 public:
  vgui_point3D() {}
  vgui_point3D(float x_, float y_, float z_) : vgui_soview3D(), x(x_), y(y_), z(z_) {}

  virtual void draw() const;
  virtual vcl_ostream& print(vcl_ostream&) const;
  vcl_string type_name() const { return "vgui_point3D"; }

  float x,y,z;
};


//: 3-dimensional line segment.
class vgui_lineseg3D : public vgui_soview3D
{
 public:
  vgui_lineseg3D() {}
  vgui_lineseg3D( float x0_, float y0_, float z0_,
                  float x1_, float y1_, float z1_ );

  virtual void draw() const;
  virtual vcl_ostream& print(vcl_ostream&) const;
  vcl_string type_name() const { return "vgui_lineseg3D"; }

  float x0, y0, z0, x1, y1, z1;
};


//: 3-dimensional line segment.
class vgui_triangle3D : public vgui_soview3D
{
 public:
  vgui_triangle3D() {}
  vgui_triangle3D( float x0_, float y0_, float z0_,
                   float x1_, float y1_, float z1_,
                   float x2_, float y2_, float z2_ );

  virtual void draw() const;
  virtual vcl_ostream& print(vcl_ostream&) const;
  vcl_string type_name() const { return "vgui_triangle3D"; }

  float x0, y0, z0, x1, y1, z1, x2, y2, z2;
};


#endif // vgui_soview3D_h_
