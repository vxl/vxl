// This is core/vgui/vgui_event_condition.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_event_condition.h for a description of this file.


#include "vgui_event_condition.h"

#include <vcl_sstream.h>
#include <vgui/vgui_event.h>

//----------------------------------------------------------------------------
// Initialise event condition and check for impossible events.
void vgui_event_condition::init(vgui_key k, vgui_key a,
                                vgui_button b, vgui_modifier m,
                                bool is_pressed, bool is_on, event_types h)
{
  on = is_on;
  pressed = is_pressed;
  key = k;
  ascii_char = a;
  button = b;
  modifier = m;
  how_checked = h;

  // key should always be lower case.
  if (k > 0 && k < 32)
  {
    // Convert control character to lower case and force the control
    // modifier.
    key = vgui_key(k + 'a' -1);
    modifier = vgui_CTRL;
  }

  if (k >= 'A' && k <= 'Z')
  {
    // Convert upper to lower case and force the shift modifier.
    key = vgui_key(k + 'a' - 'A');
    modifier = vgui_SHIFT;
  }
}

//----------------------------------------------------------------------------
//: Default constructor.
vgui_event_condition::vgui_event_condition()
{
  init(vgui_KEY_NULL, vgui_KEY_NULL, vgui_BUTTON_NULL, vgui_MODIFIER_NULL,
       false, false, null_event);
}

//----------------------------------------------------------------------------
//: Constructor for a key press event condition (using ascii char).
vgui_event_condition::vgui_event_condition(vgui_key ascii_code,
                                           bool is_pressed)
{
  init (vgui_KEY_NULL, ascii_code, vgui_BUTTON_NULL, vgui_MODIFIER_NULL,
        is_pressed, true, ascii_char_event);
}

//----------------------------------------------------------------------------
//: Constructor for a key press event condition (using key and modifier).
vgui_event_condition::vgui_event_condition(vgui_key k, vgui_modifier m, bool p)
{
  init(k, vgui_KEY_NULL, vgui_BUTTON_NULL, m, p, true, key_event);
}

//----------------------------------------------------------------------------
//: Constructor for a mouse button press event condition.
vgui_event_condition::vgui_event_condition(vgui_button b, vgui_modifier m,
                                           bool p)
{
  init(vgui_KEY_NULL, vgui_KEY_NULL, b, m, p, true, mouse_event);
}

//----------------------------------------------------------------------------
bool vgui_event_condition::operator()(vgui_event const &e) const
{
  if (! on)
    return false;
  else if (e.type == vgui_KEY_PRESS)
  {
    if (how_checked == key_event)
      return pressed && e.key == key && e.modifier == modifier;
    else if (how_checked == ascii_char_event)
      return pressed && e.ascii_char == ascii_char;
    else
      return false;
  }
  else if (e.type == vgui_KEY_RELEASE)
  {
    if (how_checked == key_event)
      return !pressed && e.key == key && e.modifier == modifier;
    else if (how_checked == ascii_char_event)
      return !pressed && e.ascii_char == ascii_char;
    else
      return false;
  }
  else if (e.type == vgui_MOUSE_DOWN)
    return how_checked == mouse_event && pressed && e.button == button
    && e.modifier == modifier;
  else if (e.type == vgui_MOUSE_UP)
    return how_checked == mouse_event && !pressed && e.button == button
    && e.modifier == modifier;
  else
    return false;
}

//----------------------------------------------------------------------------
bool vgui_event_condition::operator()(vgui_key k, vgui_modifier m) const
{
  if (! on)
    return false;
  return k == key && m == modifier;
}


//----------------------------------------------------------------------------
bool vgui_event_condition::operator()(vgui_button b, vgui_modifier m) const
{
  if (! on)
    return false;
  return b == button && m == modifier;
}

//----------------------------------------------------------------------------
//: Return a string describing the event condition.
vcl_string vgui_event_condition::as_string(int  /*field_width*/) const
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
  if (vgui_key/*egcs for i386 needs cast*/(ascii_char) != vgui_KEY_NULL) {
    vcl_ostringstream s;
    s << ascii_char;
    r += s.str();
  }

  return r;
}
