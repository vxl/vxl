// This is core/vgui/vgui_drag_mixin.cxx

//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   11 Sep 99
// \brief  See vgui_drag_mixin.h for a description of this file.

#include <iostream>
#include "vgui_drag_mixin.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_event.h>

static bool debug=false;

vgui_drag_mixin::vgui_drag_mixin()
{
  last_down_button_ = vgui_BUTTON_NULL;
}

vgui_drag_mixin::~vgui_drag_mixin()
{
}

bool vgui_drag_mixin::handle(vgui_event const &event)
{
  if ((event.type==vgui_MOTION) && (last_down_button_ != vgui_BUTTON_NULL))
    return mouse_drag(event.wx, event.wy, last_down_button_, event.modifier);

  if (event.type==vgui_BUTTON_DOWN)
    last_down_button_ = event.button;

  else if (event.type==vgui_BUTTON_UP)
    last_down_button_ = vgui_BUTTON_NULL;

  return false;
}


bool vgui_drag_mixin::mouse_drag(int, int, vgui_button, vgui_modifier)
{
  if (debug) std::cerr << "vgui_drag_mixin::mouse_drag\n";
  return false;
}
