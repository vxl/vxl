#ifndef vgui_rasterpos_h_
#define vgui_rasterpos_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

void vgui_rasterpos4dv(double const X[4]);
void vgui_rasterpos2f(float x, float y);

bool vgui_rasterpos_valid();

#endif // vgui_rasterpos_h_
