//-*- c++ -*-------------------------------------------------------------------
#ifndef vgui_mfc_window_h_
#define vgui_mfc_window_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_mfc_window - vgui_mfc_window is a the mfc+ implementation of vgui_window
// .LIBRARY vgui-mfc
// .HEADER vxl Package
// .INCLUDE vgui/impl/mfc/vgui_mfc_window.h
// .FILE vgui_mfc_window.cxx
//
// .SECTION Description:
//
// vgui_mfc_window is a the mfc+ implementation of vgui_window.
// Based on vgui_glut_win.
//
// .SECTION Author:
//    24-JUL-2000  Marko Bacic, Oxford RRG
// .SECTION Modifications:
//    17-AUG-2000  Marko Bacic, Oxford RRG - Now windows can have specified size
//    23-AUG-2000  Marko Bacic, Oxford RRG - Added support for scrollbars
//
//-----------------------------------------------------------------------------

class vgui_mfc_adaptor;
class vgui_mfc_statusbar;

#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <afxwin.h>
class vgui_mfc_window : public vgui_window {
public:
  vgui_mfc_window(char const *title, unsigned w, unsigned h, int posx =-1, int posy=-1);
  vgui_mfc_window(unsigned w,unsigned h,vgui_menu const &menubar,char const *title);
  ~vgui_mfc_window();
  
  vgui_menu menubar;
  void set_menubar(vgui_menu const &m) { menubar = m; }

  vgui_mfc_adaptor *mfcwin;
  vgui_mfc_statusbar *statusbar;
  vgui_adaptor *get_adaptor();

  //
  void show();
  void hide();
  void iconify();
  void enable_hscrollbar(bool);
  void enable_vscrollbar(bool);
  void reshape(unsigned, unsigned);
  void reposition(int,int);
  void set_title(vcl_string const &);
};

#endif // vgui_mfc_window_h_
