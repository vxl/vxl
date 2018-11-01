// This is core/vgui/vgui_key.cxx
#include <iostream>
#include "vgui_key.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//:
// \file
// \brief  See vgui_key.h for a description of this file.

//----------------------------------------------------------------------------
//: Convert key from lower case to upper case if necessary.
vgui_key vgui_key_CTRL(int character)
{
  if (character >= 'A' && character <= 'Z')
    return vgui_key(character - '@');
  else
    return vgui_key(character - 'a' + 'A' - '@');
}

//----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& s, vgui_key k)
{
  int ik = int(k);
  s << ik << '/';
  if (ik < 0)
    return s << "[Bad key, code " << ik << "]";
  else if (ik < 27)
    return s << "^" << char(ik + '@');
  else if (ik < 256)
    return s << char(ik);
  else if (ik >= vgui_F1 && ik <= vgui_INSERT) {
    static char const* names[] = {
      "F1", "F2" , "F3" , "F4" , // function keys
      "F5", "F6" , "F7" , "F8" ,
      "F9", "F10", "F11", "F12",
      "CURSOR_LEFT", "CURSOR_UP", "CURSOR_RIGHT",
        "CURSOR_DOWN", // cursor movement
      "PAGE_UP", "PAGE_DOWN",
      "HOME", "END",
      "DELETE", "INSERT"  // other things
    };
    return s << names[ik - int(vgui_F1)];
  } else
    return s << "[Bad key, code " << ik << "]";
}
