// This is oxl/vgui/vgui_text_put.h
#ifndef vgui_text_put_h_
#define vgui_text_put_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   19 Oct 99
// \brief  Function for rendering text at the current raster position.

//: Put the given text at the current raster position.
// \param size Approximate point size of the text. Current
// implementation only has 10pt and 24pt Times New Roman.
void vgui_text_put(char const *s, unsigned size=24);

#endif // vgui_text_put_h_
