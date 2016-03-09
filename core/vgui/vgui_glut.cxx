// This is core/vgui/vgui_glut.cxx
#include "vgui_glut.h"
//:
// \file
// \author fsm

#include <vcl_compiler.h>
#include <cstdlib>

extern "C"
void vgui_glut_exit(int x)
{
  std::exit(x);
}
