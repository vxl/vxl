// This is oxl/vgui/vgui_glut.cxx
#include "vgui_glut.h"
//:
// \file
// \author fsm

#include <vcl_cstdlib.h>

extern "C" 
void vgui_glut_exit(int x)
{
  vcl_exit(x);
}
