// This is oxl/vgui/vgui_event.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
// \brief  See vgui_event.h for a description of this file.
//
// \verbatim
//  Modifications:
//    11-SEP-1999 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_event.h"
#include <vcl_iostream.h>
#include <vul/vul_get_timestamp.h>

void vgui_event::init() {
  type = vgui_EVENT_NULL;
  button = vgui_BUTTON_NULL;
  key = vgui_KEY_NULL;
  modifier = vgui_MODIFIER_NULL;
  wx = 0;
  wy = 0;
  {
    static int secs_0 = -1;
    static int msecs_0 = -1;
    int secs_now, msecs_now;
    vul_get_timestamp(secs_now, msecs_now);
    if (secs_0 == -1 && msecs_0 == -1)
      secs_0 = secs_now, msecs_0 = msecs_now;
    timestamp = 1000*(secs_now - secs_0) + (msecs_now - msecs_0);
  }
  origin = 0;
  timer_id = 0;
  str = "";
  user = 0;
  data = 0;
}

vgui_event::vgui_event() {
  init();
}

vgui_event::vgui_event(vgui_event_type etype) {
  init();
  type = etype;
}

bool vgui_event::modifier_is_down(int mods) const {
  return (mods & modifier) == mods;
}

double vgui_event::secs_since(vgui_event const& e) const {
  return (this->timestamp - e.timestamp) * 1e-3;
}

long vgui_event::usecs_since(vgui_event const& e) const {
  return long(this->timestamp - e.timestamp) * 1000;
}

//------------------------------------------------------------------------------

// formatted stream output operators below.

static struct {
  vgui_event_type t;
  char const *name;
} fsm_event_table[] = {
#if defined(macro)
# error blah
#endif
#define macro(e) {vgui_ ## e,#e}
  // doing it this way means we don't rely on the event types being
  // enummed in any particular order (the code had that particular
  // bug before I changed it) -- fsm.
  macro(EVENT_NULL),
  macro(ENTER),
  macro(LEAVE),
  macro(BUTTON_DOWN),
  macro(BUTTON_UP),
  macro(MOTION),
  macro(KEY_PRESS),
  macro(KEY_RELEASE),
  macro(RESHAPE),
  macro(TIMER),
  macro(DRAW),
  macro(DRAW_OVERLAY),
  macro(STRING),
  macro(HSCROLL),
  macro(VSCROLL),
  macro(DESTROY),
  macro(OTHER)
#undef macro
};
static const int fsm_event_table_size = sizeof(fsm_event_table)/sizeof(fsm_event_table[0]);

vcl_ostream& operator<<(vcl_ostream& s, vgui_event_type t)
{
  for (int i=0; i<fsm_event_table_size; ++i)
    if (fsm_event_table[i].t == t)
      return s << fsm_event_table[i].name;
  return s << "[" __FILE__ " : bad event, code " << int(t) << "]";
}

vcl_ostream& operator<<(vcl_ostream& s, vgui_event const& e)
{
  s << "[type:" << e.type;
  if (e.key != vgui_KEY_NULL) s << ", key:" << vgui_key(e.key);
  if (e.button != vgui_BUTTON_NULL) s << ", button:" << e.button;
  if (e.modifier != vgui_MODIFIER_NULL) s << ", modifiers:" << vgui_modifier(e.modifier);
  s << ", w(" << e.wx << "," << e.wy << ")";
  s << ", time:" << e.timestamp << "ms";
  if (e.str != "") s << ", vcl_string:\"" << e.str << "\"";
  return s << "]";
};

//------------------------------------------------------------------------------
//: Returns true if events are the same.
//  Isn't this what the compiler would have generated anyway?
//  moreover, the compiler-generated one wouldn't need to be
//  updated when the fields are changed. fsm@robots.
bool operator==(vgui_event const& a, vgui_event const& b)
{
  return  a.type    == b.type &&
          a.button  == b.button &&
          a.key     == b.key &&
          a.modifier== b.modifier &&
          a.wx      == b.wx &&
          a.wy      == b.wy &&
          a.origin  == b.origin &&
          a.timer_id== b.timer_id &&
          a.str     == b.str &&
          a.user    == b.user &&
          a.data    == b.data;
}

