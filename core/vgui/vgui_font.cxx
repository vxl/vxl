// This is core/vgui/vgui_font.cxx
//=========================================================================
#include "vgui_font.h"
//:
// \file
// \brief  Font class for rendering text in OpenGL.
//
// See vgui_font.h for details.
//=========================================================================

#include <vcl_iostream.h>

//: Draw a string of font symbols.
void vgui_font::draw(const vcl_string& str, unsigned int size) const
{
  static bool first_time = true;
  if (first_time)
  {
    vcl_cerr << "The font being used can not be resized!\n";
    first_time = false;
  }
  draw(str);
}
