/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_Xm.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(Xm) {
  vul_trace;
  vgui_Xm::instance();
  vul_trace;
  return 0;
}

