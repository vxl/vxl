/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_glut_impl.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(glut) {
  vul_trace;
  new vgui_glut_impl;
  vul_trace;
  return 0;
}
