#ifndef vgui_section_render_h_
#define vgui_section_render_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/
// .NAME vgui_section_render
// .INCLUDE vgui/vgui_section_render.h
// .FILE vgui_section_render.cxx

#include <vgui/vgui_gl.h>

bool vgui_section_render(void const *pixels,
			 unsigned w, unsigned h, // Size of image.
			 float x0, float y0,  // Region of image 
			 float x1, float y1,  // to render.
			 GLenum format,
			 GLenum type,
			 bool use_vgui_rasterpos = false);

#endif // vgui_section_render_h_
