// This is oxl/vgui/vgui_event_condition.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vgui_event_condition.h"

#include <vcl_sstream.h>
#include <vgui/vgui_event.h>

bool vgui_event_condition::operator()(vgui_event const &e) const
{
  if (! on)
    return false;
  else if (e.type == vgui_KEY_PRESS)
    return pressed && e.key == key && e.modifier == modifier;
  else if (e.type == vgui_KEY_RELEASE)
    return !pressed && e.key == key && e.modifier == modifier;
  else if (e.type == vgui_MOUSE_DOWN)
    return pressed && e.button == button && e.modifier == modifier;
  else if (e.type == vgui_MOUSE_UP)
    return !pressed && e.button == button && e.modifier == modifier;
  else
    return false;
}

bool vgui_event_condition::operator()(vgui_key k, vgui_modifier m) const
{
  if (! on)
    return false;
  return k == key && m == modifier;
}


bool vgui_event_condition::operator()(vgui_button b, vgui_modifier m) const
{
  if (! on)
    return false;
  return b == button && m == modifier;
}

vcl_string vgui_event_condition::as_string(int field_width) const
{
  vcl_string r;
  if (modifier & vgui_SHIFT)
    r += "shift ";
  if (modifier & vgui_CTRL)
    r += "ctrl ";
  if (modifier & vgui_META)
    r += "meta ";
  if (modifier & vgui_ALT)
    r += "alt ";

  if (button == vgui_LEFT) r += "left";
  if (button == vgui_MIDDLE) r += "middle";
  if (button == vgui_RIGHT) r += "right";

  if (vgui_key/*egcs for i386 needs cast*/(key) != vgui_KEY_NULL) {
    vcl_ostringstream s;
    s << key;
    r += s.str();
  }

  return r;
}
