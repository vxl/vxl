// This is core/vgui/vgui_easy3D_tableau.h
#ifndef vgui_easy3D_tableau_h_
#define vgui_easy3D_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau to display three-dimensional geometric objects.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   24 Sep 99
//
//  Contains classes vgui_easy3D_tableau  vgui_easy3D_tableau_new
//
// \verbatim
//  Modifications
//   24-SEP-1999 P.Pritchett - Initial version.
//   26-APR-2002 K.Y.McGaul - Added some doxygen-style documentation.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_easy3D to vgui_easy3D_tableau.
// \endverbatim

#include "vgui_easy3D_tableau_sptr.h"
#include "vgui_displaylist3D_tableau.h"
#include "vgui_style_sptr.h"
class vgui_point3D;
class vgui_lineseg3D;
class vgui_triangle3D;

//: Tableau to display three-dimensional geometric objects.
//
//  Three-dimensional objects (see vgui_soview3D) can be added using add, or add_point, add_line.
class vgui_easy3D_tableau : public vgui_displaylist3D_tableau
{
 public:
  //: Constructor - don't use this, use vgui_easy3D_tableau_new.
  vgui_easy3D_tableau();

  //: Returns the tableau name.
  vcl_string type_name() const { return "vgui_easy3D_tableau"; }

  //: Add the given vgui_soview3D to the display.
  void add(vgui_soview3D*);

  //: Add a 3D-point at the given location.
  vgui_point3D* add_point(float x, float y, float z);

  //: Add a 3D-line with the given start and end points.
  vgui_lineseg3D* add_line(float x0, float y0, float z0,
                           float x1, float y1, float z1);

  //: Add a 3D-triangle with the given vertices.
  vgui_triangle3D* add_triangle(float x0, float y0, float z0,
                                float x1, float y1, float z1,
                                float x2, float y2, float z2);

  //: Set the colour of objects to the given RGB value.
  void set_foreground(float red, float green, float blue);

  //: Set the line width of all objects to the given width.
  void set_line_width(float new_width);

  //: Set the radius of all points to the given radius.
  void set_point_radius(float new_radius);

  void save_e3d(char const* filename);
  void load_e3d(char const* filename);


 protected:
  //: Destructor - called by vgui_easy3D_tableau_sptr.
  ~vgui_easy3D_tableau() { }

  vgui_style_sptr style_;
};

//: Create a smart-pointer to a vgui_easy3D_tableau tableau.
struct vgui_easy3D_tableau_new : public vgui_easy3D_tableau_sptr
{
  typedef vgui_easy3D_tableau_sptr base;

  //: Constructor - create an empty vgui_easy3D_tableau.
  vgui_easy3D_tableau_new() : base(new vgui_easy3D_tableau()) { }
};

#endif // vgui_easy3D_tableau_h_
