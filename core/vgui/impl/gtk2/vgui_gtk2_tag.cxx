/*
  fsm
*/
#include "vgui_gtk2.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(gtk2) {
  vul_trace;
  vgui_gtk2::instance();
  vul_trace;
  return 0;
}
