#ifndef bgui_image_tableau_sptr_h_
#define bgui_image_tableau_sptr_h_
/*
  fsm
*/
// .NAME    bgui_image_tableau_sptr
// .INCLUDE bgui/bgui_image_tableau_sptr.h
// .FILE    bgui_image_tableau_sptr.cxx

#include <vgui/vgui_tableau_sptr.h>

class bgui_image_tableau;
typedef vgui_tableau_sptr_t<bgui_image_tableau, vgui_tableau_sptr> bgui_image_tableau_sptr;

#endif // bgui_image_tableau_sptr_h_
