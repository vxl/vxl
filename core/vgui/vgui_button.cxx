// This is core/vgui/vgui_button.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief See vgui_button.h for a description of this file.

#include <iostream>
#include "vgui_button.h"
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

std::ostream& operator<<(std::ostream& s, vgui_button b)
{
  char const *spc = "";
  if (b & vgui_LEFT)   { s << spc << "left";   spc=" "; }
  if (b & vgui_MIDDLE) { s << spc << "middle"; spc=" "; }
  if (b & vgui_RIGHT)  { s << spc << "right";           }
  return s;
}
