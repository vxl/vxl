#ifndef vgui_color_text_h_
#define vgui_color_text_h_

//--------------------------------------------------------------------------------
// .NAME vgui_color_text
// .INCLUDE vgui/vgui_color_text.h
// .FILE vgui_color_text.cxx
// .SECTION Description:
//   Converts a string naming a color, eg, "blue" to a string containing
//   the RGB values, eg "0.000 0.000 1.000".
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     25-FEB-2000   Initial version
//--------------------------------------------------------------------------------

#include <vcl_string.h>

vcl_string text_to_color(vcl_string);
float red_value(vcl_string);
float green_value(vcl_string);
float blue_value(vcl_string);

#endif // vgui_color_text_h_
