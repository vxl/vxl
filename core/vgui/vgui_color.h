#ifndef vgui_color_h_
#define vgui_color_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_color - Easy functions which attempt to set the current color in both RGBA and color index mode
// .INCLUDE vgui/vgui_color.h
// .FILE vgui_color.cxx
// \author fsm@robots.ox.ac.uk

void vgui_color_white(); // 1 1 1
void vgui_color_yellow();// 1 1 0
void vgui_color_purple();// 1 0 1
void vgui_color_red();   // 1 0 0
void vgui_color_cyan();  // 0 1 1
void vgui_color_green(); // 0 1 0
void vgui_color_blue();  // 0 0 1
void vgui_color_black(); // 0 0 0

#endif // vgui_color_h_
