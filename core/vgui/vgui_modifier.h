#ifndef vgui_modifier_h_
#define vgui_modifier_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_modifier
// .INCLUDE vgui/vgui_modifier.h
// .FILE vgui_modifier.cxx

#include <vcl_iosfwd.h>

enum vgui_modifier {
  vgui_MODIFIER_NULL=0,
  vgui_CTRL =0x1, 
  vgui_SHIFT=0x2, 
  vgui_META =0x4, 
  vgui_ALT  =0x8
};

vcl_ostream& operator<<(vcl_ostream& s, vgui_modifier b);

#endif // vgui_modifier_h_
