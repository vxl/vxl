// This is ./oxl/vgui/vgui_drag_mixin.h

//:
// \file 
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
// \brief  A tableau for drag events.
//
// \verbatim
//  Modifications:
//    11-SEP-1999 P.Pritchett - Initial version.
//    26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#ifndef vgui_drag_mixin_h_
#define vgui_drag_mixin_h_
#ifdef __GNUC__
#pragma interface
#endif

#include "vgui_event.h"
#include "vgui_tableau.h"

//: A tableau for drag events 
//
// vgui_drag_mixin simplifies the implementation of interactive tableaux.
// Rather than having a switch in the tableau handle function, one implements
// the methods mouse_down, mouse_drag etc, and calls vgui_drag_mixin::do_handle(e)
// to dispatch to them.
// 
// The drag_mixin also records the last mouse button to be pressed and will send
// vgui_MOTION events to the function mouse_drag() if the mouse is being moved
// with a button depressed.
class vgui_drag_mixin {
public:
  vgui_drag_mixin(void);
  virtual ~vgui_drag_mixin(void);

  virtual bool handle(vgui_event const&);

  virtual bool mouse_drag(int, int, vgui_button, vgui_modifier);

private:
  vgui_button last_down_button_;
};

#endif // vgui_drag_mixin_h_
