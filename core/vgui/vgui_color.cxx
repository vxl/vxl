// This is core/vgui/vgui_color.cxx
//:
// \file
// \author  fsm
// \brief   See vgui_color.h for a description of this file.

#include "vgui_color.h"
#include <vgui/vgui_gl.h>
#include <vgui/vgui_macro.h>

// This assumes someone has set up the color map so that the
// first 8 indices (viewed as binary) correspond to rgb.
// Ideally, the mapping from colour to index should be
// determined at run-time and cached for each adaptor in the
// program.
static
void macro(int a, int b, int c) // cunningly, this is no longer a macro.
{
  GLboolean is_index_mode;
  vgui_macro_report_errors;
  glGetBooleanv(GL_INDEX_MODE, &is_index_mode);
  vgui_macro_report_errors;
  if (is_index_mode)
    glIndexi(4*(a) + 2*(b) + (c));
  else
    glColor3f(a, b, c);
  vgui_macro_report_errors;
}

void vgui_color_white() { macro(vgui_color_WHITE); }
void vgui_color_yellow(){ macro(vgui_color_YELLOW); }
void vgui_color_purple(){ macro(vgui_color_PURPLE); }
void vgui_color_red()   { macro(vgui_color_RED); }
void vgui_color_cyan()  { macro(vgui_color_CYAN); }
void vgui_color_green() { macro(vgui_color_GREEN); }
void vgui_color_blue()  { macro(vgui_color_BLUE); }
void vgui_color_black() { macro(vgui_color_BLACK); }
