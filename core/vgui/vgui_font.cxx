// This is core/vgui/vgui_font.cxx
//=========================================================================
#include <iostream>
#include "vgui_font.h"
//:
// \file
// \brief  Font class for rendering text in OpenGL.
//
// See vgui_font.h for details.
//=========================================================================

#include <vcl_compiler.h>

//: Draw a string of font symbols.
void vgui_font::draw(const std::string& str, unsigned int size) const
{
  static bool first_time = true;
  if (first_time)
  {
    std::cerr << "The font being used can not be resized!\n";
    first_time = false;
  }
  draw(str);
}
