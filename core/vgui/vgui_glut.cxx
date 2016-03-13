// This is core/vgui/vgui_glut.cxx
#include <cstdlib>
#include "vgui_glut.h"
//:
// \file
// \author fsm

#include <vcl_compiler.h>

extern "C"
void vgui_glut_exit(int x)
{
  std::exit(x);
}
