// This is core/vgui/internals/vgui_rasterpos.h
#ifndef vgui_rasterpos_h_
#define vgui_rasterpos_h_
//:
// \file
// \author fsm

//: Wraps glRasterPos4dv to enable setting an "invalid" raster position.
void vgui_rasterpos4dv(double const X[4]);

//: Wraps glRasterPos2f to enable setting an "invalid" raster position.
void vgui_rasterpos2f(float x, float y);

//: Wraps glRasterPos2i to enable setting an "invalid" raster position.
void vgui_rasterpos2i(int x, int y);

//: Returns true iff the current raster position is valid.
bool vgui_rasterpos_valid();

#endif // vgui_rasterpos_h_
