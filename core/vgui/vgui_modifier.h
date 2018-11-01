// This is core/vgui/vgui_modifier.h
#ifndef vgui_modifier_h_
#define vgui_modifier_h_
//:
// \file
// \author VGG, Oxford University
// \brief  Enum of modifiers for key-strokes and mouse presses.

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Enum of modifiers for key-strokes and mouse presses.
enum vgui_modifier {
  vgui_MODIFIER_NULL=0,
  vgui_CTRL =0x1,
  vgui_SHIFT=0x2,
  vgui_META =0x4,
  vgui_ALT  =0x8
};

std::ostream& operator<<(std::ostream& s, vgui_modifier b);

#endif // vgui_modifier_h_
