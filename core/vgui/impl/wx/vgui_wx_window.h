// This is core/vgui/impl/wx/vgui_wx_window.h
#ifndef vgui_wx_window_h_
#define vgui_wx_window_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets implementation of vgui_window.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   04/05/2006 - File created. (miguelfv)
//   11/18/2019 - Modifications to eliminate gl errors and memory leaks (JLM)
// \endverbatim
//=========================================================================

#include <vgui/vgui_window.h>
class vgui_wx_adaptor;
class vgui_wx_menu;
class vgui_wx_statusbar;

#include <wx/frame.h>
#include <memory>

//-------------------------------------------------------------------------
//: wxWidgets implementation of vgui_window.
//
// Creates a top-level window in wxWidgets and provides methods for its
// management and customization.
//
// Note, the wx window subclass, wxFrame, does not pass down scrollbar events.
// It is necessary to use the subclass wxScrolledWindow, but vgui just
// needs to change the offset of the vgui_viewer2D token, not scroll the
// window. The solution seems to be to create a custom subclass of wxWindow
// to capture scroll events. TODO - JLM
//-------------------------------------------------------------------------
class vgui_wx_window
  : public vgui_window
  , public wxFrame
{
public:
  //: Constructor - create a new window.
  vgui_wx_window(int width, int height, const char * title);

  //: Constructor - create a new window with a menubar.
  vgui_wx_window(int width, int height, const vgui_menu & menubar, const char * title);

  //: Destructor.
  virtual ~vgui_wx_window();

  //: Put the given menu in the menubar (if it exists).
  virtual void
  set_menubar(const vgui_menu &);

  //: If true, activate the statusbar (if it exists).
  virtual void
  set_statusbar(bool);

  //: Set the default adaptor (if it exists) to the given vgui_adaptor.
  //  These refer to the default/current adaptor, if that makes
  //  sense. It is not a requirement that it should make sense.
  virtual void
  set_adaptor(vgui_adaptor *);

  //: Get the default adaptor (if it exists).
  virtual vgui_adaptor *
  get_adaptor();

  //: Get the status bar (if it exists).
  virtual vgui_statusbar *
  get_statusbar();

  //: Display the window.
  // the dpi_scale is set here. For wxWidgets 3.1.3, the value
  // is not valid until after the window is shown.
  //
  virtual void
  show()
  {
    this->Show();
  }

  //: Hide the window from view.
  virtual void
  hide()
  {
    this->Hide();
  }

  //: Turn the window into an icon.
  virtual void
  iconify()
  {
    this->Iconize();
  }

  //: If true, activate horizontal scrollbar (if it exists).
  virtual void
  enable_hscrollbar(bool);

  //: If true, activate vertical scrollbar (if it exists).
  virtual void
  enable_vscrollbar(bool);

  //: Change window shape to new given width and height.
  virtual void
  reshape(unsigned, unsigned);

  //: Move the window to the new given x,y position.
  virtual void
  reposition(int, int);

  //: Use the given text as the window title (if the window has a title).
  virtual void
  set_title(const std::string &);

  //: Set the position of the horizontal scrollbar, returns old position.
  virtual int
  set_hscrollbar(int pos);

  //: Set the position of the vertical scrollbar, returns old position.
  virtual int
  set_vscrollbar(int pos);

  //: add a close event on exit (may not be necessary but can't hurt)
  void
  add_close_event();

private:
  //: Catch all constructor.
  void
  init_window();

  //: vgui_adaptor associated with this window.
  vgui_wx_adaptor * adaptor_;

  //: Statusbar that vgui writes to.
  std::shared_ptr<vgui_wx_statusbar> statusbar_;

  //: wxMenuBar event handler and menu.
  vgui_wx_menu * menu_;

  //: Title of the window.
  std::string title_;
};

#endif // vgui_wx_window_h_
