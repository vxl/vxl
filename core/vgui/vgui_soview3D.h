#ifndef vgui_soview3D_h_
#define vgui_soview3D_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_soview3D - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_soview3D.h
// .FILE vgui_soview3D.cxx
//
// .SECTION Description
//
// vgui_soview3D is a class that Phil hasnt documented properly. FIXME
//
//
// Notes:
//   We use floats instead of doubles as size is a speed issue (sic.)
//
// .SECTION Author
//              Philip C. Pritchett, 24 Mar 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include "vgui_soview.h"

class vgui_soview3D : public vgui_soview {
public:
  vgui_soview3D();
  virtual ~vgui_soview3D() {}
};


class vgui_point3D : public vgui_soview3D {
public:
  vgui_point3D() {};
  vgui_point3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};

  virtual void draw();
  virtual vcl_ostream& print(vcl_ostream&) const;
  vcl_string type_name() const { return "vgui_point3D"; }

  float x,y,z;
};



class vgui_lineseg3D : public vgui_soview3D {
public:
  vgui_lineseg3D() {};

  virtual void draw();
  virtual vcl_ostream& print(vcl_ostream&) const;
  vcl_string type_name() const { return "vgui_lineseg3D"; }

  float x0, y0, z0, x1, y1, z1;
};

#endif // vgui_soview3D_h_
