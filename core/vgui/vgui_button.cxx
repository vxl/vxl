// This is ./oxl/vgui/vgui_button.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file
// \brief See vgui_button.h for a description of this file.

#include "vgui_button.h"
#include <vcl_iostream.h>

vcl_ostream& operator<<(vcl_ostream& s, vgui_button b)
{
  char const *spc = "";
  if (b & vgui_LEFT)   { s << spc << "left";   spc=" "; }
  if (b & vgui_MIDDLE) { s << spc << "middle"; spc=" "; }
  if (b & vgui_RIGHT)  { s << spc << "right";  spc=" "; }
  return s;
}
