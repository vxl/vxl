// This is core/vgui/vgui_viewer2D_tableau.h
#ifndef vgui_viewer2D_tableau_h_
#define vgui_viewer2D_tableau_h_
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   14 Sep 99
// \brief  Tableau with functions to view 2D objects (eg, zoom, drag, centre).
//
//  Contains classes:  vgui_viewer2D_tableau  vgui_viewer2D_tableau_new
//
// \verbatim
//  Modifications
//    23-AUG-2000 Marko Bacic, Oxford RRG -- Added support for scroll bars
//    07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//    01-OCT-2002 K.Y.McGaul - Moved vgui_viewer2D to vgui_viewer2D_tableau.
//    23-DEC-2002 J.L. Mundy - Modified scrollbar support to reflect scale
//                             and centering.
// \endverbatim

#include <vgui/vgui_drag_mixin.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_viewer2D_tableau_sptr.h>

#include "dll.h"

//: Tableau with functions to view 2D objects (eg, zoom, drag, centre).
class vgui_viewer2D_tableau : public vgui_wrapper_tableau, public vgui_drag_mixin
{
 public:
  //: Constructor - don't use this, use vgui_viewer2D_tableau_new.
  //  Takes a child tableau as parameter.
  vgui_viewer2D_tableau(vgui_tableau_sptr const&);

  //: Handle all events sent to this tableau.
  //  In particular, uses gestures from the user to zoom, drag and centre
  //  the display.
  virtual bool handle(const vgui_event& event);

  //: Returns the type of this tableau ('vgui_viewer2D_tableau').
  virtual std::string type_name() const;

  //: Set the correct projection matrix for GL (take account of zoom, drag etc).
  void setup_gl_matrices();

  //: Modify the token to zoom in by the given factor, about the given (x,y).
  //  (x, y) are in viewport coordinates.
  void zoomin(float zoom_factor, int x, int y);

  //: Modify the token to zoom out by the given factor, about the given (x,y).
  //  (x, y) are in viewport coordinates.
  void zoomout(float zoom_factor, int x, int y);

  //: Centre the image with the given width and height inside the tableau.
  void center_image(int w, int h);

  //: Find the image (if it exists) and center it inside the tableau.
  void center_event();

  //: Called when the user presses a mouse button in the rendering area.
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

  //: Called when the user drags a mouse inside the rendering area.
  bool mouse_drag(int x, int y,  vgui_button button, vgui_modifier modifier);

  //: Called when the user releases a mouse button in the rendering area.
  bool mouse_up(int x, int y,  vgui_button button, vgui_modifier modifier);

  //: Called when the user presses the '?' key.
  bool help();

  //: Called when the user presses a key.
  bool key_press(int x, int y, vgui_key key, vgui_modifier);

  //: Data on the current state of vgui_viewer2D_tableau (eg the amount we are zoomed).
  struct token_t
  {
    float scaleX;
    float scaleY;
    float offsetX;
    float offsetY;
    token_t() : scaleX(1.0f), scaleY(1.0f), offsetX(0.0f), offsetY(0.0f) { }
  } token;

  // static data
  static vgui_DLLDATA const void * const CENTER_EVENT;


  bool nice_points;
  bool nice_lines;

  enum {normal_zoom, smooth_zoom} zoom_type;

  bool panning;
  bool smooth_zooming;
  bool sweep_zooming;
  bool sweep_next;
  float prev_x,prev_y;
  float zoom_x,zoom_y;
  float new_x,new_y;
  float zoom_factor;
  int npos_x;
  int npos_y;
 protected:
  //:get the image size
  bool image_size(int& width, int& height);

  //: Destructor - called by vgui_viewer2D_tableau_sptr.
 ~vgui_viewer2D_tableau();
};

//: Create a smart-pointer to a vgui_viewer2D_tableau tableau.
struct vgui_viewer2D_tableau_new : public vgui_viewer2D_tableau_sptr
{
  //: Create a smart-pointer to a vgui_viewer2D_tableau tableau.
  //  Takes a child tableau as parameter.
  vgui_viewer2D_tableau_new(vgui_tableau_sptr const& that) :
    vgui_viewer2D_tableau_sptr(new vgui_viewer2D_tableau(that)) { }
};

#endif // vgui_viewer2D_tableau_h_
