// This is core/vgui/vgui_text_put.h
#ifndef vgui_text_put_h_
#define vgui_text_put_h_
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 99
// \brief  Function for rendering text at the current raster position.

//: Put the given text at the current raster position.
// \param size Approximate point size of the text.
// with glut, the choices are:
//  times roman 24, 10
//  helvectia 18, 12
// without glut, the only choice is:
// bitmap font 8x13
void vgui_text_put(const char* s, unsigned size=24);
// the return value is the total pixel width spanned by the char string
int vgui_text_length(const char* s, unsigned size=24);
int vgui_char_width(unsigned char s, unsigned size=24);

#endif // vgui_text_put_h_
