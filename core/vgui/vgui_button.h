// This is core/vgui/vgui_button.h
#ifndef vgui_button_h_
#define vgui_button_h_
//:
// \file
// \brief Enumeration of possible mouse buttons.
//
//  WIN32 NOTE: The middle mouse button does not produce a middle mouse button
//  event by default on Windows.  If you are not getting middle mouse button
//  events then look at Start->Settings->Control Panel->Mouse.  Check on
//  the "Button Actions" tab that your middle mouse button is set to "Middle"
//  - kym.

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Enumeration of possible mouse buttons.
enum vgui_button {
  vgui_BUTTON_NULL=0,
  vgui_LEFT  =0x1,
  vgui_MIDDLE=0x2,
  vgui_RIGHT =0x4
};

std::ostream& operator<<(std::ostream& s, vgui_button b);

#endif // vgui_button_h_
