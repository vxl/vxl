/*
  fsm
*/
#include "vgui_qt.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(qt) {
  vul_trace;
  vgui_qt::instance();
  vul_trace;
  return 0;
}
