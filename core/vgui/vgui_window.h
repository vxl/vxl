// This is core/vgui/vgui_window.h
#ifndef vgui_window_h_
#define vgui_window_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  An abstract representation of a system window.
// \author fsm
//
// \verbatim
//  Modifications
//   07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//   23-DEC-2002 J.L. Mundy - added set methods for scroll bars
// \endverbatim

class vgui_menu;
class vgui_adaptor;
class vgui_statusbar;
#include <vcl_string.h>

//: An abstract representation of a system window.
//
//  It is not necessarily the application's top level window, but
//  it may have a menubar, or the concept of an application menubar,
//  which is accessed using set_menubar.
//
//  Possibly not all GUI toolkits using this representation will have
//  menus, statusbars, scrollbars or titles, though there wouldn't be much
//  point in implementing this if you couldn't at least display an OpenGL
//  area (vgui_adaptor) in your window.
class vgui_window
{
 public:
  //: Default constructor.
  vgui_window() {}

  //: Destructor.
  virtual ~vgui_window() {}

  //: Put the given menu in the menubar (if it exists).
  virtual void set_menubar(vgui_menu const &);

  //: If true, activate the statusbar (if it exists).
  virtual void set_statusbar(bool) {}

  //: Set the default adaptor (if it exists) to the given vgui_adaptor.
  //  These refer to the default/current adaptor, if that makes
  //  sense. It is not a requirement that it should make sense.
  virtual void set_adaptor(vgui_adaptor *);

  //: Get the default adaptor (if it exists).
  virtual vgui_adaptor *get_adaptor();

  //: Get the status bar (if it exists).
  virtual vgui_statusbar *get_statusbar();

  //: Display the window.
  virtual void show();

  //: Hide the window from view.
  virtual void hide();

  //: Turn the window into an icon.
  virtual void iconify();

  //: If true, activate horizontal scrollbar (if it exists).
  virtual void enable_hscrollbar(bool);

  //: If true, activate vertical scrollbar (if it exists).
  virtual void enable_vscrollbar(bool);

  //: Change window shape to new given width and height.
  virtual void reshape(unsigned, unsigned);

  //: Move the window to the new given x,y position.
  virtual void reposition(int, int);

  //: Use the given text as the window title (if the window has a title).
  virtual void set_title(vcl_string const &);

  //: Set the position of the horizontal scrollbar, returns old position
  virtual int set_hscrollbar(int pos);

  //: Set the position of the vertical scrollbar, returns old position
  virtual int set_vscrollbar(int pos);
};

#endif // vgui_window_h_
