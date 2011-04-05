// This is core/vgui/impl/win32/vgui_win32_statusbar.cxx

#include "vgui_win32_statusbar.h"
#include <commctrl.h>

int vgui_win32_statusbar::statusBarID = 200;

vgui_win32_statusbar::vgui_win32_statusbar(HWND hwndParent, int numPanes)
  : statusbuf(new vgui_statusbuf(this)), out(statusbuf),
     hwndParent_(hwndParent), numPanes_(numPanes)
{
#ifdef _WIN64
  hwnd_ = CreateWindowEx(
         0L,                                       // no extended styles
         STATUSCLASSNAME,                          // status bar
         "",                                       // no text
         WS_CHILD|WS_BORDER|WS_VISIBLE,            // styles
         -100, -100, 10, 10,                       // x, y, cx, cy
         hwndParent,                               // parent window
         (HMENU)statusBarID,                       // window ID
         (HINSTANCE)GetWindowLong(hwndParent, GWLP_HINSTANCE), // instance
         NULL);                                    // window data
  if ( hwnd_ == NULL )
    MessageBox(NULL, TEXT("Fail to create status bar"), TEXT("Error"),
               MB_ICONERROR | MB_OK);
#else
  hwnd_ = CreateWindowEx(
         0L,                                       // no extended styles
         STATUSCLASSNAME,                          // status bar
         "",                                       // no text
         WS_CHILD|WS_BORDER|WS_VISIBLE,            // styles
         -100, -100, 10, 10,                       // x, y, cx, cy
         hwndParent,                               // parent window
         (HMENU)statusBarID,                       // window ID
         (HINSTANCE)GetWindowLong(hwndParent, GWL_HINSTANCE), // instance
         NULL);                                    // window data
  if ( hwnd_ == NULL )
    MessageBox(NULL, TEXT("Fail to create status bar"), TEXT("Error"),
               MB_ICONERROR | MB_OK);
#endif //_WIN64
}

vgui_win32_statusbar::~vgui_win32_statusbar()
{
  delete statusbuf;
  DestroyWindow(hwnd_);
}

int vgui_win32_statusbar::write(const char* text, int n)
{
  static bool start_new = false;
  if (n == 1) { // If user appends one char, we cache it unless it's a  '\n'
    if (text[0] == '\n') {
      // We are at the end of the message. Set a bool so we know next time
      // to clear the buffer before we start a new message.
      start_new = true;
    }
    else if (start_new == true){
      SendMessage(hwnd_, SB_SETTEXT, 0, (LPARAM)(LPSTR)linebuffer.c_str());
      linebuffer = "";
      linebuffer += text[0];
      start_new = false;
    }
    else
      linebuffer += text[0];
  }
  else {
    linebuffer.append(text, n);
    if (linebuffer.find('\n')) {
      SendMessage(hwnd_, SB_SETTEXT, 0, (LPARAM)(LPSTR)linebuffer.c_str());
      linebuffer = "";
    }
  }

  return n;
}

int vgui_win32_statusbar::write(const char* text)
{
  linebuffer = text;
  SendMessage(hwnd_, SB_SETTEXT, 0, (LPARAM)(LPSTR)linebuffer.c_str());

  return 1;
}
