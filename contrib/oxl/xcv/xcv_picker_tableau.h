// This is oxl/xcv/xcv_picker_tableau.h
#ifndef xcv_picker_tableau_h_
#define xcv_picker_tableau_h_
//:
// \file
// \author K.Y.McGaul
// \brief  Tableau to pick points and lines.
//
//  The difference between this picker tableau and vgui_rubberbander is that
//  this keeps control of the event loop until the object has been picked.
//  The functions pick_point/pick_line will only return once the user has
//  picked a point/line.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul   23-MAR-2001  Initial version.
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_parent_child_link.h>

// <vgui_make_sptr>
#include "xcv_picker_tableau_sptr.h"
// </vgui_make_sptr>

class xcv_picker_tableau : public vgui_tableau
{
public:

  //: Constructor, takes a child tableau.
  xcv_picker_tableau(vgui_tableau_sptr const&);
  //: Destructor.
  ~xcv_picker_tableau();
  std::string type_name() const { return "xcv_picker_tableau";}

  //: Gets a user selected point.
  bool pick_point(float* x, float* y);
  //: Gets a user selected line.
  void pick_line(float* x1, float* y1, float* x2, float* y2);
  //: Handles all events for this tableau.
  bool handle(const vgui_event&);


protected:
  vgui_parent_child_link child_tab;

private:
  //: Draw a line to help the user pick it.
  void draw_line();
  //: Get next event in the event loop.
  bool next();
  //: List of possible objects to pick.
  enum object_type {none_enum, point_enum, line_enum};
  //: Type of object we are picking.
  static object_type obj_type;

  //: Used in next() to indicate when the event has been used.
  bool use_event_;

  //: Start and end points for line:
  float pointx1, pointy1, pointx2, pointy2;
  //: Whether this is the first (start) or second (end) point being selected.
  bool FIRSTPOINT;

  //: Coordinates for point:
  float pointx, pointy;
  //: True if picked by left mouse button, else false.
  bool point_ret;

  //: True if we have completed picking the object.
  bool picking_completed;
};


// <vgui_make_sptr>
struct xcv_picker_tableau_new : public xcv_picker_tableau_sptr
{
  typedef xcv_picker_tableau_sptr base;
  xcv_picker_tableau_new(vgui_tableau_sptr const& arg1000) : base(new xcv_picker_tableau(arg1000)) { }
};
// <vgui_make_sptr>

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS xcv_picker_tableau.
