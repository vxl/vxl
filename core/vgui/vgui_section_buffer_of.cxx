// This is oxl/vgui/vgui_section_buffer_of.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vgui_section_buffer_of.h"

#define VGUI_SECTION_BUFFER_OF_INSTANTIATE(T) \
template class vgui_section_buffer_of<T >

VGUI_SECTION_BUFFER_OF_INSTANTIATE(GLubyte);
VGUI_SECTION_BUFFER_OF_INSTANTIATE(vgui_pixel_rgb888);
VGUI_SECTION_BUFFER_OF_INSTANTIATE(vgui_pixel_rgba8888);
