// This is ./oxl/vgui/vgui_event.h

//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
// \brief  The vgui_event class encapsulates the events handled by the vgui system.
//
// \verbatim
// Modifications:
// 16 Sep 1999  fsm@robots. various.
//  5 Oct 1999  fsm@robots. replaced (x,y) by (wx,wy) and (ux,uy).
// 10 Oct 1999  pcp         added timestamp
// 20 Oct 1999  awf         Changed timestamp to int.
// 19 Oct 1999  fsm@robots. added pointer to adaptor.
//  1 Nov 1999  fsm@robots. events now use viewport, not window coordinates.
// 28 Nov 1999  fsm@robots. added vcl_string event.
// 22 Aug 2000  Marko Bacic. added support for scroll bar events
// \endverbatim

#ifndef vgui_event_h_
#define vgui_event_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl_string.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_button.h>
#include <vgui/vgui_modifier.h>
class vgui_adaptor;

enum vgui_event_type {
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
  vgui_OTHER,
  vgui_EVENT_MAX
};
vcl_ostream& operator<<(vcl_ostream& s, vgui_event_type e);

//: The vgui_event class encapsulates the events handled by the vgui system.

class vgui_event {
public:
  vgui_event();
  vgui_event(vgui_event_type);

  vgui_event_type type;
  vgui_button button;
  vgui_key key;
  vgui_modifier modifier;

  //: Position of the mouse pointer in viewport coordinates when the event occurred.
  int wx,wy;

  //: Timestamp in milliseconds since app started.
  int timestamp;

  //: The adaptor from which the event came.
  vgui_adaptor *origin;

  //:
  // For an event of type vgui_TIMER, this field holds the name
  // that was given when the timer request was posted.
  int timer_id;

  //: A vcl_string message, for an event of type vgui_STRING.
  // An event of type vgui_STRING implies that
  // this field contains some sort of textual message. The exact
  // encoding of these messages is unspecified; the sender and the
  // receiver may use any protocal they like. Caveat : as a
  // corollary there is no guarantee that one protocal will not
  // clash with another.
  vcl_string str;

  //: type and data for events of type vgui_OTHER.
  // The fields user and data are used only when the event type is vgui_OTHER.
  // The 'user' field must uniquely identify the type of event, in the
  // sense that once the user field is known, the 'data' field can be
  // safely cast to point to the client data (type).
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
