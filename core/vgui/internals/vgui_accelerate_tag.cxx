/*
  capes@robots.ox.ac.uk
*/

#include <vgui/vgui_gl.h>
#include "vgui_accelerate.h"
#include "vgui_accelerate_x11.h"
#include "vgui_accelerate_mfc.h"
#include <vgui/vgui_tag.h>
#include <vbl/vbl_trace.h>

#ifdef VGUI_MESA
vgui_tag(accelerate_x11) {
  vbl_trace;
  vgui_accelerate::register_accelerator(new vgui_accelerate_x11, 1);
  vbl_trace;
  return 0;
}
#endif
#ifdef WIN32
vgui_tag(accelerate_mfc) {
  vbl_trace;
  vgui_accelerate::register_accelerator(new vgui_accelerate_mfc, 1);
  vbl_trace;
  return 0;
}
#endif
