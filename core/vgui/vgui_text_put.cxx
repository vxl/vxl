// This is core/vgui/vgui_text_put.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 99
// \brief  See vgui_text_put.h for a description of this file.


#include "vgui_text_put.h"
#include <vgui/vgui_gl.h>
#include <vgui/vgui_config.h>

#if defined(HAS_GLUT) && defined(VGUI_USE_GLUT)
#include <vgui/vgui_glut.h>
void vgui_text_put(char const *str, unsigned size)
{
  if (size > 17) {
    for (unsigned k=0; str[k]; ++k)
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[k]);
  } else {
    for (unsigned k=0; str[k]; ++k)
      glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, str[k]);
  }
}
#else
#include <vgui/vgui_font_bitmap.h>
void vgui_text_put(char const *str, unsigned int size)
{
  vgui_font_bitmap font(vgui_font_bitmap::BITMAP_FONT_8_13);
  font.draw(str);
}
#endif
