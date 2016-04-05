// This is core/vgui/impl/win32/vgui_win32.h

#ifndef vgui_win32_h_
#define vgui_win32_h_

// The Win32 Application Programming Interfaces (API) implementation of
// vgui_toolkit.
// author: Lianqing Yu

// Modifications:
// July 30, 2009              Initial version

// Notes:
// vgui_win32 acts like CWinThread and CWinApp in MFC. It performs program
// initialization/termination, process message loops, and deal with
// command line arguments.

// Enable Windows XP (or later) visual styles for common controls
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <vgui/vgui_toolkit.h>
#include <windows.h>

// TODO issues:
// 1. add_event()


// Provide functions for (a) controlling event loop,
//                       (b) translating Win32's event system to vgui_event.

class vgui_win32 : public vgui_toolkit
{
 public:
  ~vgui_win32();

  // Window management functions.
  vgui_window* get_current_window() { return current_window; }
  void set_current_window(HWND);
  void remove_current_window();
  int find_window(HWND);
  void dump_window_stack();

  // Dialog box management functions
  vgui_dialog_impl* get_current_dialog() { return current_dialog; }
  void remove_current_dialog();

  // Singleton method instance.
  static vgui_win32* instance();

  virtual void init(int &, char **);
  virtual void uninit();

  // Returns the name of the toolkit
  virtual std::string name() const { return "win32"; }

  virtual vgui_window* produce_window(int width, int height,
                                      vgui_menu const &menubar,
                                      char const *title);

  virtual vgui_window* produce_window(int width, int height,
                                      char const *title);

  virtual vgui_dialog_impl* produce_dialog(char const *name);

  virtual vgui_dialog_extensions_impl* produce_dialog_extension(char const *name);

  virtual void run();
  virtual void run_one_event();
  virtual void run_till_idle();
  virtual void flush();
  virtual void add_event(vgui_event const &);
  virtual void quit();

 protected:
  vgui_win32();

  // Process command line arguments
  BOOL ProcessShellCommand(int argc, char **argv);

  // Pump a message from the thread's message queue and process it.
  BOOL PumpMessage();

  static vgui_win32 *instance_; // or put this member into instance().

  // The four arguments of WinMain() are put here as class members.
  HINSTANCE hInstance_; // application instance handle
  HINSTANCE hPrevInstance_;
  PSTR      szCmdLine_;
  int       iCmdShow_;
  char     *szAppName_;

  // Save pointers to vgui_win32_window for access and free at app exit.
  std::vector<vgui_window*> windows_to_delete;
  vgui_window* current_window;

  // Save pointer to vgui_dialog_impl for access from dialog procedure.
  std::vector<vgui_dialog_impl*> dialogs_to_delete;
  vgui_dialog_impl* current_dialog;
};

// Global window procedure and dialog procedure
LRESULT CALLBACK globalWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK globalDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK globalTableauProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif // vgui_win32_h_
