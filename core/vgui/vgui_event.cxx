// This is core/vgui/vgui_event.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief  See vgui_event.h for a description of this file.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   11 Sep 99
//
// \verbatim
//  Modifications
//   11-SEP-1999 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_event.h"
#include <vcl_iostream.h>
#include <vul/vul_get_timestamp.h>

//----------------------------------------------------------------------------
//: Initialise default event.
void vgui_event::init()
{
  type = vgui_EVENT_NULL;
  button = vgui_BUTTON_NULL;
  key = vgui_KEY_NULL;
  ascii_char = vgui_KEY_NULL;
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

//----------------------------------------------------------------------------
//: Constructor - create an event of the given type.
vgui_event::vgui_event(vgui_event_type etype)
{
  init();
  type = etype;
}

//----------------------------------------------------------------------------
//: Convert the given key to lower case and use that to set the key.
//  I added this to avoid the complication of doing this conversion in each
//  GUI impl - kym.
void vgui_event::set_key(vgui_key c)
{
  if (c < 32)
  {
    // Convert control character to lower case character
    key = vgui_key(c + 'a' -1);
  }
  else if (c >= 'A' && c <= 'Z')
  {
    // Convert upper case to lower case
    key = vgui_key(c + 'a' - 'A');
  }
  else
  {
    key = vgui_key(c);
  }
}

//----------------------------------------------------------------------------
bool vgui_event::modifier_is_down(int mods) const
{
  return (mods & modifier) == mods;
}

//----------------------------------------------------------------------------
double vgui_event::secs_since(vgui_event const& e) const
{
  return (this->timestamp - e.timestamp) * 1e-3;
}

//----------------------------------------------------------------------------
long vgui_event::usecs_since(vgui_event const& e) const
{
  return long(this->timestamp - e.timestamp) * 1000;
}

//----------------------------------------------------------------------------
static struct
{
  vgui_event_type t;
  char const *name;
} fsm_event_table[] = {
#if defined(macro)
# error blah
#endif
#define macro(e) {vgui_ ## e,#e}
  // doing it this way means we don't rely on the event types being
  // enummed in any particular order (the code had that particular
  // bug before I changed it). fsm.
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

//-----------------------------------------------------------------------------
vcl_ostream& operator<<(vcl_ostream& s, vgui_event_type t)
{
  for (int i=0; i<fsm_event_table_size; ++i)
    if (fsm_event_table[i].t == t)
      return s << fsm_event_table[i].name;
  return s << "[" __FILE__ " : bad event, code " << int(t) << ']';
}

//-----------------------------------------------------------------------------
vcl_ostream& operator<<(vcl_ostream& s, vgui_event const& e)
{
  s << "[type:" << e.type;
  if (e.key != vgui_KEY_NULL) s << ", key:" << vgui_key(e.key);
  if (e.ascii_char != 0) s << ", ascii_char: " << vgui_key(e.ascii_char);
  if (e.button != vgui_BUTTON_NULL) s << ", button:" << e.button;
  if (e.modifier != vgui_MODIFIER_NULL) s << ", modifiers:" << vgui_modifier(e.modifier);
  s << ", w(" << e.wx << ',' << e.wy << ')'
    << ", time:" << e.timestamp << "ms";
  if (e.str != "") s << ", vcl_string:\"" << e.str << "\"";
  return s << ']';
}

//-----------------------------------------------------------------------------
//: Returns true if events are the same.
//  Isn't this what the compiler would have generated anyway?
//  moreover, the compiler-generated one wouldn't need to be
//  updated when the fields are changed. fsm.
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

