/*
  fsm@robots.ox.ac.uk
*/
#include "vgui_gtk.h"
#include <vgui/vgui_tag.h>
#include <vbl/vbl_trace.h>

vgui_tag(gtk) {
  vbl_trace;
  vgui_gtk::instance();
  vbl_trace;
  return 0;
}

