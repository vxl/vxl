// This is oxl/vgui/vgui_easy3D.h
#ifndef vgui_easy3D_h_
#define vgui_easy3D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Sep 99
// \brief  Tableau to display three-dimensional geometric objects.
//
//  Contains classes: vgui_easy3D  vgui_easy3D_new
//
// \verbatim
//  Modifications:
//    24-SEP-1999  P.Pritchett - Initial version.
//    26-APR-2002  K.Y.McGaul - Added some doxygen-style documentation.
// \endverbatim

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
  //: Constructor - don't use this, use vgui_easy3D_new.
  vgui_easy3D();

  //: Returns the tableau name.
  vcl_string type_name() const { return "vgui_easy3D"; }

  //: Add the given vgui_soview3D to the display.
  void add(vgui_soview3D*);

  //: Add a 3D-point at the given location.
  vgui_point3D* add_point(float x, float y, float z);

  //: Add a 3D-line with the given start and end points.
  vgui_lineseg3D* add_line(float x0, float y0, float z0, float x1, float y1, float z1);

  //: Set the colour of objects to the given RGB value.
  void set_foreground(float red, float green, float blue);

  //: Set the line width of all objects to the given width.
  void set_line_width(float new_width);

  //: Set the radius of all points to the given radius.
  void set_point_radius(float new_radius);

  void save_e3d(char const* filename);
  void load_e3d(char const* filename);


 protected:
  ~vgui_easy3D() { }

  float fg[3];
  float line_width;
  float point_size;
};

//: Create a smart-pointer to a vgui_easy3D tableau.
struct vgui_easy3D_new : public vgui_easy3D_sptr
{
  typedef vgui_easy3D_sptr base;

  //: Constructor - create an empty vgui_easy3D.
  vgui_easy3D_new() : base(new vgui_easy3D()) { }
};

#endif // vgui_easy3D_h_
