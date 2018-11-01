// This is core/vgui/vgui_event_condition.h
#ifndef vgui_event_condition_h_
#define vgui_event_condition_h_
//:
// \file
// \author fsm
// \brief  Represent and recognise simple event conditions.
//
// \verbatim
//  Modifications
//   04-OCT-2002 K.Y.McGaul - Added doxygen style documentation.
//                          - Check for impossible events in new init function.
//                          - key is now always lower case.
//   22-OCT-2002 A.Fitzgibbon & K.Y.McGaul - Added constructor for ascii_char
//                                           event conditions.
//   05-DEC-2002 K.Y.McGaul - Added Awf's comments from mailing list.
// \endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_key.h>
#include <vgui/vgui_button.h>
#include <vgui/vgui_modifier.h>
class vgui_event;

//: Represent and recognise simple event conditions.
//
//  This makes it easy to change the key/mouse combination that causes
//  one's tableau to do something.
//
//  The key is now always lower case. ascii_char contains the
//  actual character returned by the keyboard.  To construct a
//  vgui_event_condition to detect a SHIFT+b event you can do either:
// \code
//       vgui_event_condition my_ec(vgui_key('b'), vgui_SHIFT);
// \endcode
//  or
// \code
//       vgui_event_condition my_ec(vgui_key('B');
// \endcode
//
//  The point about vgui_event_condition is that it should not be
//  constructed just before you check the event, but placed in a
//  standard place in the object to
//
//  (a) Make it easy to see the list of handled events
//
//  (b) Make it easy to change the keys/mouse gestures to which
//      an action is bound.  For example, if my 3D viewer tableau
//      uses left mouse to rotate in 3D, and I wish to put it into
//      a 2D zoomer tableau which uses left mouse to zoom, I need
//      change one of them. If you don't use vgui_event_condition,
//      you need to edit the code, which means other people can't
//      use left mouse.
//
//  Concrete example:  my_tableau spins a 3D model
//
// \code
//  struct my_tableau
//  {
//    void handle(vgui_event);
//
//    vgui_event_condition c_rotate;     // event which initiates rotating
//    vgui_event_condition c_translate;
//    vgui_event_condition c_scale;
//    vgui_event_condition c_zoom;
//  };
// \endcode
//
//  Note that the event conditions are stored in the object, not hard-coded
//  in the handle routine.  This means they can be changed by client code
//  without modifying my_tableau.cxx
//
//  One might think it would be better to have all tableaux to use unique
//  event codes, and maybe it is for the core tableaux, but we can't have
//  someone use ctrl+shift+3rd button because it's the only one free, when
//  for their application (movie player shuttle action?) it may be the
//  application's most natural action, and should be on left-mouse.

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
  // This type of event condition will never be triggered.
  vgui_event_condition();

  //: Constructor for a key press event condition (using ascii char).
  vgui_event_condition(vgui_key ascii_code, bool is_pressed = true);

  //: Constructor for a key press event condition (using key and modifier).
  vgui_event_condition(vgui_key k, vgui_modifier m,  bool is_pressed = true);

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
  std::string as_string(int field_width = 0) const;
};

#endif // vgui_event_condition_h_
