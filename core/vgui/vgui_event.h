// This is core/vgui/vgui_event.h
#ifndef vgui_event_h_
#define vgui_event_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
// \brief  vgui_event class encapsulates the events handled by the vgui system.
//
// \verbatim
//  Modifications
//   16-Sep-1999  fsm. various.
//    5-Oct-1999  fsm. replaced (x,y) by (wx,wy) and (ux,uy).
//   10-Oct-1999  pcp         added timestamp
//   20-Oct-1999  awf         Changed timestamp to int.
//   19-Oct-1999  fsm. added pointer to adaptor.
//    1-Nov-1999  fsm. events now use viewport, not window coordinates.
//   28-Nov-1999  fsm. added vcl_string event.
//   22-Aug-2000  Marko Bacic. added support for scroll bar events
//   04-Oct-2002  K.Y.McGaul - Added set_key() to make sure vgui_key is now
//                             always lower case to save confusion.
//                           - Added ascii_char value to vgui_event.
// \endverbatim

#include <vcl_string.h>
#include <vcl_iosfwd.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_button.h>
#include <vgui/vgui_modifier.h>
class vgui_adaptor;

enum vgui_event_type
{
  vgui_EVENT_NULL = 0,
  vgui_ENTER,
  vgui_LEAVE,
  vgui_BUTTON_DOWN,  /* */ vgui_MOUSE_DOWN = vgui_BUTTON_DOWN, vgui_MOUSE_PRESS = vgui_BUTTON_DOWN,
  vgui_BUTTON_UP,    /* */ vgui_MOUSE_UP = vgui_BUTTON_UP, vgui_MOUSE_RELEASE = vgui_BUTTON_UP,
  vgui_MOTION,       /* */ vgui_MOUSE_MOTION = vgui_MOTION,
  vgui_KEY_PRESS,    /* */ vgui_KEY_DOWN = vgui_KEY_PRESS,
  vgui_KEY_RELEASE,  /* */ vgui_KEY_UP = vgui_KEY_RELEASE,
  vgui_RESHAPE,
  vgui_TIMER,
  vgui_DRAW,
  vgui_DRAW_OVERLAY, /* */ vgui_OVERLAY_DRAW = vgui_DRAW_OVERLAY,
  vgui_STRING,
  vgui_HSCROLL,
  vgui_VSCROLL,
  vgui_DESTROY,
  vgui_IDLE,
  vgui_OTHER,
  vgui_EVENT_MAX
};

vcl_ostream& operator<<(vcl_ostream& s, vgui_event_type e);

//: The vgui_event class encapsulates the events handled by the vgui system.
//
// For key presses with modifiers the following standards apply:
// \verbatim
//         a   modifier = vgui_NULL   key = 'a'  ascii_char = 'a'
//    CTRL+a   modifier = vgui_CTRL   key = 'a'  ascii_char = '^A'
//   SHIFT+a   modifier = vgui_SHIFT  key = 'a'  ascii_char = 'A'
// \endverbatim
//
// We have decided to make it a standard that key is always lower case for
// simplicity.  In particular people have been defining impossible
// vgui_event_conditions, eg key='A', modifier=NULL (where NULL is the
// default modifier) and then wondering why SHIFT+a doesn't work.
//
// A new data type has been added (ascii_char) which holds the actual
// key stroke pressed by the user.
class vgui_event
{
 public:
  //: Constructor - create a default event.
  vgui_event() { init(); }

  //: Constructor - create an event of the given type.
  vgui_event(vgui_event_type);

  //: The type of event (key press, mouse motion, etc).
  vgui_event_type type;

  //: Mouse button used (if it is a mouse event).
  vgui_button button;

  //: The key pressed in lower case (if it is a key event).
  vgui_key key;

  //: Convert given key to lower case and use that to set key.
  void set_key(vgui_key c);

  //: Which modifiers are pressed during the event (NULL, CTRL, SHIFT).
  vgui_modifier modifier;

  //: The actual key stroke pressed by the user.
  vgui_key ascii_char;

  //: Position of the mouse pointer in viewport coordinates when event occurred.
  int wx,wy;

  //: Timestamp in milliseconds since app started.
  int timestamp;

  //: The adaptor from which the event came.
  vgui_adaptor *origin;

  //: If the event is a timer event, this holds the ID.
  // For an event of type vgui_TIMER, this field holds the name
  // that was given when the timer request was posted.
  int timer_id;

  //: A vcl_string message, for an event of type vgui_STRING.
  //  An event of type vgui_STRING implies that
  //  this field contains some sort of textual message. The exact
  //  encoding of these messages is unspecified; the sender and the
  //  receiver may use any protocol they like. Caveat : as a
  //  corollary there is no guarantee that one protocol will not
  //  clash with another.
  vcl_string str;

  //: Type and data for events of type vgui_OTHER.
  //  The fields user and data are used only when the event type is vgui_OTHER.
  //  The 'user' field must uniquely identify the type of event, in the
  //  sense that once the user field is known, the 'data' field can be
  //  safely cast to point to the client data (type).
  void const *user;
  void const *data;

  // methods
  bool modifier_is_down(int) const;
  double secs_since(vgui_event const &) const;
  long usecs_since(vgui_event const &) const;

 private:
  void init();
};

bool operator==(vgui_event const& a, vgui_event const& b);
vcl_ostream& operator<<(vcl_ostream&, vgui_event const&);

#endif // vgui_event_h_
