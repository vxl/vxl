#ifndef vgui_button_h_
#define vgui_button_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_button
// .INCLUDE vgui/vgui_button.h
// .FILE vgui_button.cxx

#include <vcl_iosfwd.h>

enum vgui_button {
  vgui_BUTTON_NULL=0,
  vgui_LEFT  =0x1,
  vgui_MIDDLE=0x2,
  vgui_RIGHT =0x4
};

vcl_ostream& operator<<(vcl_ostream& s, vgui_button b);

#endif // vgui_button_h_
