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

#if 1
#include <vgui/vgui_font_sptr.h>
//#include <vgui/vgui_font_textured.h>
#include <vgui/vgui_font_bitmap.h>
void vgui_text_put(char const *str, unsigned int size)
{
  //static vgui_font_sptr font = new vgui_font_textured("./fonts/times_new.bmf");
  static vgui_font_sptr font = new vgui_font_bitmap;
  font->draw(str, size);
}
#else
#include <vcl_iostream.h>
void vgui_text_put(char const *str, unsigned /*size*/)
{
  vcl_cerr << __FILE__ ":!defined(HAS_GLUT):" << str << vcl_endl;
}
#endif

#endif
