// This is oxl/vgui/internals/vgui_rasterpos.h
#ifndef vgui_rasterpos_h_
#define vgui_rasterpos_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

void vgui_rasterpos4dv(double const X[4]);
void vgui_rasterpos2f(float x, float y);

bool vgui_rasterpos_valid();

#endif // vgui_rasterpos_h_
