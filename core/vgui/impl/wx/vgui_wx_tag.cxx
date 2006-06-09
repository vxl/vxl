// This is core/vgui/impl/wx/vgui_wx_tag.cxx
//=========================================================================
//:
// \file
// \brief Implement tag function to allow the vgui_wx toolkit registration.
//
// See vgui_tag.h and vgui_register_all.cxx for details.
//=========================================================================

#include "vgui_wx.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

vgui_tag(wx) {
  vul_trace;
  vgui_wx::instance();
  vul_trace;
  return 0;
}
