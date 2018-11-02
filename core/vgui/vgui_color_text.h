// This is core/vgui/vgui_color_text.h
#ifndef vgui_color_text_h_
#define vgui_color_text_h_
//:
// \file
// \brief  Converts a string naming a colour to the RGB values.
// \author K.Y.McGaul
//
//  e.g. "blue" is converted to "0.000 0.000 1.000".
//
// \verbatim
//  Modifications
//   K.Y.McGaul     25-FEB-2000   Initial version
//   26-APR-2002  K.Y.McGaul - Added doxygen style comments.
// \endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Convert a string naming a colour to a string of the RGB values.
std::string text_to_color(const std::string&);
//: Given a string naming a colour, return its red value.
float red_value(const std::string&);
//: Given a string naming a colour, return its green value.
float green_value(const std::string&);
//: Given a string naming a colour, return its blue value.
float blue_value(const std::string&);

#endif // vgui_color_text_h_
