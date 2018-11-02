/*
  capes@robots.ox.ac.uk
*/

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_gl.h>
#include "vgui_accelerate.h"
#include "vgui_accelerate_x11.h"
#include <vgui/vgui_tag.h>
#include <vul/vul_trace.h>

#if VGUI_MESA
vgui_tag(accelerate_x11) {
  vul_trace;
  vgui_accelerate::register_accelerator(new vgui_accelerate_x11, 1);
  vul_trace;
  return 0;
}
#endif

#if 0 // #ifdef HAS_MFC
#include "vgui_accelerate_mfc.h"

vgui_tag(accelerate_mfc) {
  vul_trace;
  vgui_accelerate::register_accelerator(new vgui_accelerate_mfc, 1);
  vul_trace;
  return 0;
}
#endif
