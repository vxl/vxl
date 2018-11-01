// This is core/vgui/vgui_drag_mixin.h
#ifndef vgui_drag_mixin_h_
#define vgui_drag_mixin_h_
//:
// \file
// \brief  Simplifies the handling of drag events.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
//
// \verbatim
//  Modifications
//   11-SEP-1999 P.Pritchett - Initial version.
//   26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#include "vgui_event.h"
#include "vgui_tableau.h"

//: Simplifies the handling of drag events.
//
//  A drag event occurs when the user moves the mouse with one of the
//  mouse buttons pressed down.  In VGUI there is no vgui_DRAG event (there
//  is only vgui_MOTION for when the mouse is moving).  So if you want to
//  capture drag events you may find this class handy.
//
//  The drag_mixin records the last mouse button to be pressed and will
//  send vgui_MOTION events to the function mouse_drag() if the mouse is being
//  moved  with a button depressed.
//
//  An easy way to use vgui_drag_mixin is to derive your tableau from
//  vgui_drag_tableau. You would then be able to implement mouse_drag and
//  all the other function like key_press, mouse_down in vgui_tableau to
//  handle events.
class vgui_drag_mixin
{
  //: Mouse button the user is pressing (vgui_BUTTON_NULL if none is pressed).
  vgui_button last_down_button_;

 public:
  //: Constructor - create a default vgui_drag_mixin.
  vgui_drag_mixin(void);

  //: Destructor.
  virtual ~vgui_drag_mixin(void);

  //: Look for drag events and pass them to mouse_drag().
  virtual bool handle(vgui_event const&);

  //: Called when the user is dragging the mouse.
  virtual bool mouse_drag(int, int, vgui_button, vgui_modifier);
};

#endif // vgui_drag_mixin_h_
