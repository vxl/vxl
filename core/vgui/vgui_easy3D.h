#ifndef vgui_easy3D_h_
#define vgui_easy3D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_easy3D - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_easy3D.h
// .FILE vgui_easy3D.cxx
//
// .SECTION Description:
//
// vgui_easy3D is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 24 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include "vgui_easy3D_sptr.h"
#include "vgui_displaylist3D.h"
class vgui_point3D;
class vgui_lineseg3D;

class vgui_easy3D : public vgui_displaylist3D
{
public:
  vgui_easy3D();

  vcl_string type_name() const { return "vgui_easy3D"; }

  void add(vgui_soview3D*);

  vgui_point3D* add_point(float x, float y, float z);
  vgui_lineseg3D* add_line(float x0, float y0, float z0, float x1, float y1, float z1);

  void set_foreground(float, float, float);
  void set_line_width(float);
  void set_point_radius(float);

  void save_e3d(char const* filename);
  void load_e3d(char const* filename);


protected:
  ~vgui_easy3D() { }

  float fg[3];
  float line_width;
  float point_size;
};

struct vgui_easy3D_new : public vgui_easy3D_sptr {
  typedef vgui_easy3D_sptr base;
  vgui_easy3D_new() : base(new vgui_easy3D()) { }
};

#endif // vgui_easy3D_h_
