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
// \endverbatim
//=========================================================================

#include <vgui/vgui_window.h>
class vgui_wx_adaptor;
class vgui_wx_menu;
class vgui_wx_statusbar;

#include <wx/frame.h>

//-------------------------------------------------------------------------
//: wxWidgets implementation of vgui_window.
//
// Creates a top-level window in wxWidgets and provides methods for its
// management and customization.
//-------------------------------------------------------------------------
class vgui_wx_window : public vgui_window
{
 public:
  //: Constructor - create a new window.
  vgui_wx_window(int width, int height, const char* title);

  //: Constructor - create a new window with a menubar.
  vgui_wx_window(int width,
                 int height,
                 const vgui_menu& menubar,
                 const char* title);

  //: Destructor.
  virtual ~vgui_wx_window();

  //: Put the given menu in the menubar (if it exists).
  virtual void set_menubar(vgui_menu const&);

  //: If true, activate the statusbar (if it exists).
  virtual void set_statusbar(bool);

  //: Set the default adaptor (if it exists) to the given vgui_adaptor.
  //  These refer to the default/current adaptor, if that makes
  //  sense. It is not a requirement that it should make sense.
  virtual void set_adaptor(vgui_adaptor*);

  //: Get the default adaptor (if it exists).
  virtual vgui_adaptor* get_adaptor();

  //: Get the status bar (if it exists).
  virtual vgui_statusbar* get_statusbar();

  //: Display the window.
  virtual void show() { frame_->Show(); }

  //: Hide the window from view.
  virtual void hide() { frame_->Hide(); }

  //: Turn the window into an icon.
  virtual void iconify() { frame_->Iconize(); }

  //: If true, activate horizontal scrollbar (if it exists).
  virtual void enable_hscrollbar(bool);

  //: If true, activate vertical scrollbar (if it exists).
  virtual void enable_vscrollbar(bool);

  //: Change window shape to new given width and height.
  virtual void reshape(unsigned, unsigned);

  //: Move the window to the new given x,y position.
  virtual void reposition(int, int);

  //: Use the given text as the window title (if the window has a title).
  virtual void set_title(std::string const&);

  //: Set the position of the horizontal scrollbar, returns old position.
  virtual int set_hscrollbar(int pos);

  //: Set the position of the vertical scrollbar, returns old position.
  virtual int set_vscrollbar(int pos);

 private:
  //: Catch all constructor.
  void init_window();

  //: The wxWidgets window.
  wxFrame* frame_;

  //: vgui_adaptor associated with this window.
  vgui_wx_adaptor* adaptor_;

  //: Statusbar that vgui writes to.
  vgui_wx_statusbar* statusbar_;

  //: wxMenuBar event handler and menu.
  vgui_wx_menu* menu_;

  //: Title of the window.
  std::string title_;
};

#endif // vgui_wx_window_h_
