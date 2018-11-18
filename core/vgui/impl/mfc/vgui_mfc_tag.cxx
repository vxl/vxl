// This is core/vgui/impl/mfc/vgui_mfc_tag.cxx
#include "vgui_mfc.h"
//:
// \file
// \author  awf@robots.ox.ac.uk

#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(mfc) {
  vul_trace;
  vgui_mfc::instance();
  vul_trace;
  return 0;
}
