//-*- c++ -*-------------------------------------------------------------------
#ifndef vgui_window_h_
#define vgui_window_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_window
// .INCLUDE vgui/vgui_window.h
// .FILE vgui_window.cxx
//
// .SECTION Description
//    vgui_window is an abstract representation of a system window.
//    It is not necessarily the application's top level window, but
//    it may have a menubar, or the concept of an application menubar,
//    which is accessed using set_menubar.
//
// .SECTION Author
//     fsm@robots.ox.ac.uk

class vgui_menu;
class vgui_adaptor;
class vgui_statusbar;

#include <vcl_string.h>

class vgui_window {
public:
  vgui_window();
  virtual ~vgui_window();
  
  virtual void set_menubar(vgui_menu const &);
  virtual void set_statusbar(bool) {}

  // These refer to the default/current adaptor, if that makes
  // sense. It is not a requirement that it should make sense.
  virtual void set_adaptor(vgui_adaptor *);
  virtual vgui_adaptor *get_adaptor();
  virtual vgui_statusbar *get_statusbar();

  //
  virtual void show();
  virtual void hide();
  virtual void iconify();
  virtual void enable_hscrollbar(bool);
  virtual void enable_vscrollbar(bool);
  virtual void reshape(unsigned, unsigned);
  virtual void reposition(int, int);

  virtual void set_title(vcl_string const &);
};

#endif // vgui_window_h_
