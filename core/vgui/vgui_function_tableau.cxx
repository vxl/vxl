// This is oxl/vgui/vgui_function_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   25 Nov 99
// \brief  See vgui_function_tableau.h for a description of this file.

#include "vgui_function_tableau.h"
#include <vgui/vgui_event.h>

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_function_tableau_new.
//  Creates a default vgui_function_tableau.
vgui_function_tableau::vgui_function_tableau()
  : draw_(0)
  , mouse_up_(0)
  , mouse_down_(0)
  , motion_(0)
  , key_press_(0)
  , help_(0)
{
}

//----------------------------------------------------------------------------
//: Destructor - called by vgui_function_tableau_sptr.
vgui_function_tableau::~vgui_function_tableau()
{
}

bool vgui_function_tableau::redraw = false;

//----------------------------------------------------------------------------
//: Handles all events by passing them to the appropriate functions.
bool vgui_function_tableau::handle(const vgui_event& event) {

  if (event.type==vgui_DRAW && draw_) {
    return (*draw_)(event);
  }

  else if (event.type==vgui_BUTTON_DOWN && mouse_down_) {
    bool retv = (*mouse_down_)(event);
    if (redraw) {
      post_redraw();
      redraw = false;
    }
    return retv;
  }

  else if (event.type==vgui_BUTTON_UP && mouse_up_) {
    bool retv = (*mouse_up_)(event);
    if (redraw) {
      post_redraw();
      redraw = false;
    }
    return retv;
  }

  else if (event.type==vgui_MOTION && motion_) {
    bool retv = (*motion_)(event);
    if (redraw) {
      post_redraw();
      redraw = false;
    }
    return retv;
  }

  else if (event.type==vgui_KEY_PRESS) {

    if (event.key == '?' && help_)
      return (*help_)(event);

    else if (key_press_) {
      bool retv = (*key_press_)(event);
      if (redraw) {
        post_redraw();
        redraw = false;
      }
      return retv;
    }
  }
  return false;
}
