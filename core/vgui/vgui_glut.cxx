// This is core/vgui/vgui_glut.cxx
#include <cstdlib>
#include "vgui_glut.h"
//:
// \file
// \author fsm

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

extern "C"
void vgui_glut_exit(int x)
{
  std::exit(x);
}
