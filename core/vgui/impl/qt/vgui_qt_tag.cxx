/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_qt.h"
#include <vgui/vgui_tag.h>
#include <vbl/vbl_trace.h>

vgui_tag(qt) {
  vbl_trace;
  vgui_qt::instance();
  vbl_trace;
  return 0;
}

