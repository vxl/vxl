/*
  fsm
*/
#include "vgui_Fl.h"
#include <vul/vul_trace.h>
#include <vgui/vgui_tag.h>

vgui_tag(fltk) {
  vul_trace;
  vgui_Fl::instance();
  vul_trace;
  return 0;
}
