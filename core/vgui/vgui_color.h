// This is core/vgui/vgui_color.h
#ifndef vgui_color_h_
#define vgui_color_h_
//:
// \file
// \author fsm
// \brief  Easy functions which attempt to set the current color in both RGBA and color index mode

#define vgui_color_WHITE  1, 1, 1
#define vgui_color_YELLOW 1, 1, 0
#define vgui_color_PURPLE 1, 0, 1
#define vgui_color_RED    1, 0, 0
#define vgui_color_CYAN   0, 1, 1
#define vgui_color_GREEN  0, 1, 0
#define vgui_color_BLUE   0, 0, 1
#define vgui_color_BLACK  0, 0, 0

void vgui_color_white();
void vgui_color_yellow();
void vgui_color_purple();
void vgui_color_red();
void vgui_color_cyan();
void vgui_color_green();
void vgui_color_blue();
void vgui_color_black();

#endif // vgui_color_h_
