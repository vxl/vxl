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
//  Contains classes: vgui_event_condition
//
// \verbatim
//  Modifications:
//   04-OCT-2002 K.Y.McGaul - Added doxygen style documentation.
//                          - Check for impossible events in new init function.
//                          - key is now always lower case.
//    22-OCT-2002 A.Fitzibbon & K.Y.McGaul - Added constructor for ascii_char
//                           event conditions.
// \verbatim

#include <vcl_string.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_button.h>
#include <vgui/vgui_modifier.h>
class vgui_event;

//: Represent and recognise simple event conditions.
//
//  This makes it easy to change the key/mouse combination that causes
//  one's tableau to do something.
//
//  The key is now always lower case to save confusion.  
struct vgui_event_condition
{
  enum event_types { null_event, mouse_event, ascii_char_event, key_event };

  bool on;
  bool pressed;
  vgui_key key;
  vgui_key ascii_char;
  vgui_button button;
  vgui_modifier modifier;
  event_types how_checked;

  //: Initialise event condition and check for impossible events.
  void init(vgui_key k, vgui_key ascii_char, vgui_button b,
    vgui_modifier m, bool is_pressed, bool is_on, event_types use_event);

  //: Constructor - create a default event condition.
  vgui_event_condition();

  //: Constructor for a key press event condition (using ascii char).
  vgui_event_condition(vgui_key ascii_code, bool is_pressed = true);

  //: Constructor for a key press event condition (using key and modifier).
  vgui_event_condition(vgui_key k, vgui_modifier m, 
  bool is_pressed = true);

  //: Constructor for a mouse button press event condition.
  vgui_event_condition(vgui_button b, vgui_modifier m = vgui_MODIFIER_NULL, 
    bool is_pressed = true);

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
