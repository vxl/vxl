/*
  capes@robots.ox.ac.uk
*/

#include <vcl_compiler.h>
#include <vgui/vgui_gl.h>
#include "vgui_accelerate.h"
#include "vgui_accelerate_x11.h"
#include "vgui_accelerate_mfc.h"
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
#ifdef VCL_WIN32
vgui_tag(accelerate_mfc) {
  vul_trace;
  vgui_accelerate::register_accelerator(new vgui_accelerate_mfc, 1);
  vul_trace;
  return 0;
}
#endif
