// This is core/vgui/vgui_section_render.h
#ifndef vgui_section_render_h_
#define vgui_section_render_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <vgui/vgui_gl.h>
#include <vbl/vbl_array_1d.h>
//: Render the pixels onto the frame buffer
//
// This renders the portion of the image within (x0,y0)-(x1,y1). The
// whole image is rendered if (x0,y0)==(0,0) and
// (x1,y1)==(w,h). (Actually, for efficiency, only the portion
// overlapping the viewport is ever rendered.)  \a format and \a type
// encode the data encoding in \a pixels. \a pixels is assumed to
// store a row-ordered set of byte-aligned values.
//
bool vgui_section_render(void const *pixels,
                         unsigned w, unsigned h, // Size of image.
                         float x0, float y0,  // Region of image
                         float x1, float y1,  // to render.
                         GLenum format,
                         GLenum type ,
                         bool hardware_map = false,
                         vbl_array_1d<float>* fLmap=0,
                         vbl_array_1d<float>* fRmap=0,
                         vbl_array_1d<float>* fGmap=0,
                         vbl_array_1d<float>* fBmap=0,
                         vbl_array_1d<float>* fAmap=0);
                         

#endif // vgui_section_render_h_
