// This is oxl/vgui/vgui_event_condition.h
#ifndef vgui_event_condition_h_
#define vgui_event_condition_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  Represent and recognise simple event conditions.
//
//  Contains classes: vgui_event

#include <vcl_string.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_button.h>
#include <vgui/vgui_modifier.h>
class vgui_event;

//: Represent and recognise simple event conditions.
//  This makes it easy to change the key/mouse combination that causes
//  one's tableau to do something.
struct vgui_event_condition
{
  // We use bitfields to avoid the charge that these things take
  // up space. Everything should fit into a 32bit word. In any
  // case, the machine code needed to check for the condition
  // represented would be much longer than four 32bit words. To
  // save even more space, delete any comments and unnecessary
  // blanks that remain in your code.
  bool on : 1;
  bool pressed : 1;
  vgui_key key : 16;
  vgui_button button : 3;
  vgui_modifier modifier : 4;

  vgui_event_condition()
    : on(false) { } //: key(vgui_KEY_NULL), button(vgui_BUTTON_NULL), modifier(vgui_MODIFIER_NULL) { }

  vgui_event_condition(vgui_key k, vgui_modifier m = vgui_MODIFIER_NULL, bool p = true)
    : on(true), pressed(p), key(k), button(vgui_BUTTON_NULL), modifier(m) { }

  vgui_event_condition(vgui_button b, vgui_modifier m = vgui_MODIFIER_NULL, bool p = true)
    : on(true), pressed(p), key(vgui_KEY_NULL), button(b), modifier(m) { }

  void enable(bool v = true) { on = v; }
  void disable(bool v = true) { on = !v; }

  //: E.g. if (c_mouse_spin(e))
  bool operator()(vgui_event const &e) const;
  bool operator()(vgui_key k, vgui_modifier m) const;
  bool operator()(vgui_button b, vgui_modifier m) const;

  //: Text representation such as "shift-middle" or "ctrl-K".
  // If field_width is supplied, pad to that width.
  vcl_string as_string(int field_width = 0) const;
};

#endif // vgui_event_condition_h_
