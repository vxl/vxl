//-*- c++ -*-------------------------------------------------------------------
#ifndef vgui_text_put_h_
#define vgui_text_put_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_text_put
// .INCLUDE vgui/vgui_text_put.h
// .FILE vgui_text_put.cxx
//
// .SECTION Description
// Function for rendering text at the current raster position.
//
// .SECTION Author
//    Philip C. Pritchett, RRG, University of Oxford
//    Created: 19 Oct 99

//: put the given text at the current raster position.
void vgui_text_put(char const *s);

#endif // vgui_text_put_h_
