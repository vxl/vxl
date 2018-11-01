// This is core/vgui/impl/mfc/vgui_mfc_app_init.h
#ifndef vgui_mfc_app_init_h_
#define vgui_mfc_app_init_h_
//:
// \file
// \author  Oxford RRG
// \brief   Initializes the MFC CWinApp object.

class vgui_mfc_app;

//: Initialize MFC CWinApp object.
// Place an object of type vgui_mfc_app_init in your main program
// in order to set up the MFC stuff;
class vgui_mfc_app_init
{
 public:
  vgui_mfc_app_init();
  ~vgui_mfc_app_init();

 protected:
  vgui_mfc_app* p;
};

#endif // vgui_mfc_app_init_h_
