/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
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

void vgui_color_white() { macro(1, 1, 1); }
void vgui_color_yellow(){ macro(1, 1, 0); }
void vgui_color_purple(){ macro(1, 0, 1); }
void vgui_color_red()   { macro(1, 0, 0); }
void vgui_color_cyan()  { macro(0, 1, 1); }
void vgui_color_green() { macro(0, 1, 0); }
void vgui_color_blue()  { macro(0, 0, 1); }
void vgui_color_black() { macro(0, 0, 0); }
