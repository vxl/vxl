// This is oxl/vgui/vgui_soview3D.h
#ifndef vgui_soview3D_h_
#define vgui_soview3D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Mar 99
// \brief  3-dimensional geometric objects.
// 
// Contains classes:  vgui_soview3D  vgui_point3D  vgui_linseg3D
// Notes:  We use floats instead of doubles as size is a speed issue (sic.)
//

#include "vgui_soview.h"

//: 3-dimensional geometric objects.
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
  vgui_point3D() {};
  vgui_point3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};

  virtual void draw();
  virtual vcl_ostream& print(vcl_ostream&) const;
  vcl_string type_name() const { return "vgui_point3D"; }

  float x,y,z;
};


//: 3-dimensional line segment.
class vgui_lineseg3D : public vgui_soview3D
{
 public:
  vgui_lineseg3D() {};

  virtual void draw();
  virtual vcl_ostream& print(vcl_ostream&) const;
  vcl_string type_name() const { return "vgui_lineseg3D"; }

  float x0, y0, z0, x1, y1, z1;
};

#endif // vgui_soview3D_h_
