// This is ./oxl/vgui/vgui_modifier.cxx
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_modifier.h"
#include <vcl_iostream.h>

//:
// \file
// \author VGG, Oxford University
// \brief  See vgui_modifier.h for a description of this file.

vcl_ostream& operator<<(vcl_ostream& s, vgui_modifier m)
{
  char const *spc = "";
  if (m & vgui_CTRL)  { s << spc << "ctrl";  spc=" "; }
  if (m & vgui_SHIFT) { s << spc << "shift"; spc=" "; }
  if (m & vgui_META)  { s << spc << "meta";  spc=" "; }
  if (m & vgui_ALT)   { s << spc << "alt";   spc=" "; }
  return s;
}
