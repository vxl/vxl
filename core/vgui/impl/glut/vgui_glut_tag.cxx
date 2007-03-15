/*
  fsm
*/
#include "vgui_glut_impl.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(glut) {
  vul_trace;
  vgui_glut_impl::instance();
  vul_trace;
  return 0;
}
