// This is ./oxl/vgui/vgui_easy3D.h

//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Sep 99
// \brief  Tableau to display three-dimensional geometric objects.
//
// \verbatim
//  Modifications:
//    24-SEP-1999  P.Pritchett - Initial version.
//    26-APR-2002  K.Y.McGaul - Added some doxygen-style documentation.
// \endverbatim

#ifndef vgui_easy3D_h_
#define vgui_easy3D_h_
#ifdef __GNUC__
#pragma interface
#endif

#include "vgui_easy3D_sptr.h"
#include "vgui_displaylist3D.h"
class vgui_point3D;
class vgui_lineseg3D;

//: Tableau to display three-dimensional geometric objects.
//
//  Three-dimensional objects (see vgui_soview3D) can be added using add, or add_point, add_line.
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
