// This is oxl/vgui/vgui_section_render.h
#ifndef vgui_section_render_h_
#define vgui_section_render_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <vgui/vgui_gl.h>

bool vgui_section_render(void const *pixels,
                         unsigned w, unsigned h, // Size of image.
                         float x0, float y0,  // Region of image
                         float x1, float y1,  // to render.
                         GLenum format,
                         GLenum type,
                         bool use_vgui_rasterpos = false);

#endif // vgui_section_render_h_
