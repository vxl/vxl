// This is ./oxl/vgui/vgui_button.h

//:
// \file
// \brief Enumeration of possible mouse buttons.

#ifndef vgui_button_h_
#define vgui_button_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl_iosfwd.h>

//: Enumeration of possible mouse buttons.
enum vgui_button {
  vgui_BUTTON_NULL=0,
  vgui_LEFT  =0x1,
  vgui_MIDDLE=0x2,
  vgui_RIGHT =0x4
};

vcl_ostream& operator<<(vcl_ostream& s, vgui_button b);

#endif // vgui_button_h_
