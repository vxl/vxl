// This is ./oxl/vgui/vgui_color_text.h

//:
// \file
// \author K.Y.McGaul
// \brief  Converts a string naming a colour to the RGB values.
//
//  e.g. "blue" is converted to "0.000 0.000 1.000".
//
// \verbatim
//  Modifications:
//    K.Y.McGaul     25-FEB-2000   Initial version
//    26-APR-2002  K.Y.McGaul - Added doxygen style comments.
// \endverbatim

#ifndef vgui_color_text_h_
#define vgui_color_text_h_

#include <vcl_string.h>

//: Convert a string naming a colour to a string of the RGB values.
vcl_string text_to_color(vcl_string);
//: Given a string naming a colour, return its red value.
float red_value(vcl_string);
//: Given a string naming a colour, return its green value.
float green_value(vcl_string);
//: Given a string naming a colour, return its blue value.
float blue_value(vcl_string);

#endif // vgui_color_text_h_
