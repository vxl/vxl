// This is oxl/vgui/vgui_event_condition.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  See vgui_event_condition.h for a description of this file.


#include "vgui_event_condition.h"

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vgui/vgui_event.h>

//----------------------------------------------------------------------------
// Initialise event condition and check for impossible events.
void vgui_event_condition::init(vgui_key k, vgui_button b, vgui_modifier m,  
  bool p, bool is_on)
{
  on = is_on;
  key = k;
  button = b;
  modifier = m;
  pressed = p;

  // key should always be lower case.
  if (k > 0 && k < 32) 
  {
    // Convert control character to lower case
    key = vgui_key(k + 'a' -1);
    // (operator!= doesn't work for bitfields with gcc 2.95)
    if (! (modifier == vgui_CTRL))
    {
      vcl_cerr << "ERROR attempting to make an impossible vgui_event_condition."
        << " - key " << vgui_key(k) << " is a control character, but modifier "
        << "is not vgui_CTRL" << vcl_endl;
    }
  }

  if (k >= 'A' && k <= 'Z')
  {
    // Convert upper to lower case:
    key = vgui_key(k + 'a' - 'A');
    // (operator!= doesn't work for bitfields with gcc 2.95)
    if (! (modifier == vgui_SHIFT))
    {
      vcl_cerr << "ERROR attempting to make an impossible vgui_event_condition"
        << " - Given key " << vgui_key(k) << " is a shift character, but "
        << "modifier is not vgui_SHIFT" << vcl_endl;
    }
  } 
}
 
//----------------------------------------------------------------------------
//: Default constructor.
vgui_event_condition::vgui_event_condition()
{
  init(vgui_KEY_NULL, vgui_BUTTON_NULL, vgui_MODIFIER_NULL, false, false);
}

//----------------------------------------------------------------------------
//: Constructor for a key press event condition.
vgui_event_condition::vgui_event_condition(vgui_key k, vgui_modifier m, bool p)
{ 
  init(k, vgui_BUTTON_NULL, m, p, true);
}

//----------------------------------------------------------------------------
//: Constructor for a mouse button press event condition.
vgui_event_condition::vgui_event_condition(vgui_button b, vgui_modifier m, 
  bool p)
{
  init(vgui_KEY_NULL, b, m, p, true);
}

//----------------------------------------------------------------------------
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
