/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_gtk.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(gtk) {
  vul_trace;
  vgui_gtk::instance();
  vul_trace;
  return 0;
}

