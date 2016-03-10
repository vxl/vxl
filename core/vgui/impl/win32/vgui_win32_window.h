// This is core/vgui/impl/win32/vgui_win32_window.h
#ifndef vgui_win32_window_h_
#define vgui_win32_window_h_
//:
// \file
// \brief The Win32 Application Programming Interfaces (API) implementation of vgui_window.
// \author Lianqing Yu
// \date   July 30, 2009
//
// Modifications:
// July 30, 2009              Initial version


#include <vgui/vgui_window.h>
#include <vgui/impl/win32/vgui_win32_adaptor.h>
#include <vgui/impl/win32/vgui_win32_statusbar.h>
#include <vgui/impl/win32/vgui_win32_cmdtarget.h>

#include <windows.h>

class vgui_win32_window : public vgui_window, public vgui_win32_cmdtarget
{
 public:
  // Constructors. application instance handle and name are required
  // to create a window in Win32 programming.
  vgui_win32_window(HINSTANCE hInst, const char *appname, int width,
                    int height, vgui_menu const &menubar, char const *title);
  vgui_win32_window(HINSTANCE hInst, const char *appname, int width,
                    int height, char const *title);
  ~vgui_win32_window();

  // Public functions for accessing members.
  HWND getWindowHandle() { return hwnd_; }
  HACCEL getAccelHandle() { return hAccel_; }

 //: Put the given menu in the menubar (if it exists).
  virtual void set_menubar(vgui_menu const &);

  //: If true, activate the statusbar (if it exists).
  virtual void set_statusbar(bool);

  //: Set the default adaptor (if it exists) to the given vgui_adaptor.
  //  These refer to the default/current adaptor, if that makes
  //  sense. It is not a requirement that it should make sense.
  virtual void set_adaptor(vgui_adaptor *adaptor) { /*adaptor_ = adaptor;*/ }


  //: Get the default adaptor (if it exists).
  virtual vgui_adaptor *get_adaptor() { return adaptor_; }

  //: Get the status bar (if it exists).
  // \todo This function is not called yet.
  virtual vgui_statusbar *get_statusbar() { return statusbar_; }


  //: Display the window.
  virtual void show();

  //: Hide the window from view.
  // \todo This function is not called yet.
  virtual void hide() { ShowWindow(hwnd_, SW_HIDE); }

  //: Turn the window into an icon.
  // \todo This function is not called yet.
  virtual void iconify() { ShowWindow(hwnd_, SW_MINIMIZE); }

  //: If true, activate horizontal scrollbar (if it exists).
  virtual void enable_hscrollbar(bool);

  //: If true, activate vertical scrollbar (if it exists).
  virtual void enable_vscrollbar(bool);

  //: Change window shape to new given width and height.
  virtual void reshape(unsigned, unsigned);

  //: Move the window to the new given x,y position.
  virtual void reposition(int, int);

  //: Use the given text as the window title (if the window has a title).
  // \todo This function is not called yet.
  virtual void set_title(std::string const &t) { SetWindowText(hwnd_, t.c_str()); }

  //: Set the position of the horizontal scrollbar, returns old position
  virtual int set_hscrollbar(int pos);

  //: Set the position of the vertical scrollbar, returns old position
  virtual int set_vscrollbar(int pos);

  // Message handling function
  virtual LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  virtual LRESULT DefWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
  virtual BOOL OnCmdMsg(UINT message, WPARAM wParam, LPARAM lParam);
  // Called within message processing loop.
  void menu_dispatcher(int menuId);

#if 0
  void OnCreate();
  void OnClose();
  void OnTimer();
#endif

 protected:
  HWND  hwnd_;  //!< application window handle
  HMENU hMenu_; //!< application menu handle
  HACCEL hAccel_; // accelerator handle
  std::vector<vgui_command_sptr> callbacks; //!< commands called by menu items

 private:
  // Create window and initialize class members.
  void initWindow(HINSTANCE hInst, const char *appname, int width,
                  int height, char const *title);

  vgui_win32_adaptor *adaptor_;
  vgui_win32_statusbar *statusbar_;

  int      wx_, wy_; //!< window position (wx, wy)
  unsigned cx_, cy_; //!< and client area size (cx, cy).
  bool first_show;   //!< indicate if the member function show()
                     //!< is called for the first time

  DECLARE_MESSAGE_MAP()
};

#endif // vgui_win32_window_h_
