/*
  awf@robots.ox.ac.uk
*/
#include "vgui_mfc.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(mfc) {
  vul_trace;
  vgui_mfc::instance();
  vul_trace;
  return 0;
}

