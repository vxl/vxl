#ifndef xcv_image_tableau_sptr_h_
#define xcv_image_tableau_sptr_h_
/*
  fsm
*/
// .NAME    xcv_image_tableau_sptr
// .INCLUDE xcv/xcv_image_tableau_sptr.h
// .FILE    xcv_image_tableau_sptr.cxx

#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_image_tableau_sptr.h>

class xcv_image_tableau;
typedef vgui_tableau_sptr_t<xcv_image_tableau, vgui_image_tableau_sptr> xcv_image_tableau_sptr;

#endif // xcv_image_tableau_sptr_h_
