/*
  fsm
*/
#include "vgui_glX.h"
#include <vul/vul_trace.h>
#include <vgui/vgui_tag.h>

vgui_tag(glX) {
  vul_trace;
  vgui_glX::instance();
  vul_trace;
  return 0;
}
