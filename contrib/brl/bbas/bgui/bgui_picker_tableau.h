// This is brl/bbas/bgui/bgui_picker_tableau.h
#ifndef bgui_picker_tableau_h_
#define bgui_picker_tableau_h_
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
//  Anchored_pick_point rubberbands a line joining a first point (the anchor)
//  during the motion to select a second point. Useful for corresponding
//  pairs of points - JLM
//
// \verbatim
//  Modifications
//   K.Y.McGaul - 23-MAR-2001 - Initial version.
//   J. Mundy - Jan 9, 2005 - Added anchored pick point
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_parent_child_link.h>
#include "bgui_picker_tableau_sptr.h"

class bgui_picker_tableau : public vgui_tableau
{
 public:

  //: Constructor, takes a child tableau.
  bgui_picker_tableau(vgui_tableau_sptr const&);
  //: Destructor.
  ~bgui_picker_tableau();
  vcl_string type_name() const { return "bgui_picker_tableau";}

  //: Gets a user selected point.
  bool pick_point(float* x, float* y);

  //: Gets a user selected line.
  void pick_line(float* x1, float* y1, float* x2, float* y2);

  //: Pick a point with an anchored line indicator
  void anchored_pick_point(const float anch_x,
                           const float anch_y,
                           float* x, float* y);

  //: Set drawing style, [0 1.0] for colors
  void set_color(const float red=1.0f, const float green=1.0f,
                 const float blue=1.0f) {r = red; g = green; b = blue;}
  void set_line_width(const float width=2.0f){w = width;}

  //: Handles all events for this tableau.
  bool handle(const vgui_event&);

 protected:
  vgui_parent_child_link child_tab;

 private:
  //: Draw a line to help the user pick it.
  void draw_line();
  void draw_anchor_line();

  //: Get next event in the event loop.
  bool next();
  //: List of possible objects to pick.
  enum object_type {none_enum, point_enum, line_enum, anchor_enum};
  //: Type of object we are picking.
  static object_type obj_type;

  //: Used in next() to indicate when the event has been used.
  bool use_event_;

  //: Start and end points for line:
  float pointx1, pointy1, pointx2, pointy2;

  //: Anchor point coordinates
  float anchor_x, anchor_y;

  //: Style values
  float r, g, b, w;
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
struct bgui_picker_tableau_new : public bgui_picker_tableau_sptr
{
  typedef bgui_picker_tableau_sptr base;
  bgui_picker_tableau_new(vgui_tableau_sptr const& arg1000) : base(new bgui_picker_tableau(arg1000)) {}
};
// </vgui_make_sptr>

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS bgui_picker_tableau.
