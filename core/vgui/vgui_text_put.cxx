// This is oxl/vgui/vgui_text_put.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 19 Oct 99
//
//-----------------------------------------------------------------------------

#include "vgui_text_put.h"
#include <vgui/vgui_gl.h>

#ifdef HAS_GLUT
#include <vgui/vgui_glut.h>
void vgui_text_put(char const *str)
{
  for (unsigned k=0; str[k]; ++k)
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[k]);
}
#else
#include <vcl_iostream.h>
void vgui_text_put(char const *str)
{
  vcl_cerr << __FILE__ ":!defined(HAS_GLUT):" << str << vcl_endl;
}
#endif
