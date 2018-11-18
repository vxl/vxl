// This is core/vgui/impl/win32/vgui_win32_window.cxx

#include "vgui_win32_window.h"
#include "vgui_win32_utils.h"
#include <vgui/vgui.h>
#include <vgui/vgui_command.h>

BEGIN_MESSAGE_MAP(vgui_win32_window, vgui_win32_cmdtarget)
END_MESSAGE_MAP()

LRESULT vgui_win32_window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if ( OnCmdMsg(message, wParam, lParam) )
    return 1;
  else
    return DefWndProc(hwnd, message, wParam, lParam);
}

LRESULT vgui_win32_window::DefWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  // Note: must NOT call DefWindowProc(hwnd, message, wParam, lParam);
  return 0;
}

vgui_win32_window::vgui_win32_window(HINSTANCE hInst, const char *appName,
                                     int width, int height, vgui_menu const &menubar, char const *title)
: cx_(width), cy_(height), first_show(true)
{
  initWindow(hInst, appName, width, height, title);

  // Convert vgui_menu to Win32 menu handle (HMENU) to add it to the main window.
  hMenu_ = vgui_win32_utils::instance()->vgui_menu_to_win32ex(menubar, callbacks, &hAccel_);
  SetMenu(hwnd_, hMenu_);
}

vgui_win32_window::vgui_win32_window(HINSTANCE hInst, const char *appName,
                                     int width, int height, char const *title)
: cx_(width), cy_(height), first_show(true)
{
  initWindow(hInst, appName, width, height, title);
}

void vgui_win32_window::initWindow(HINSTANCE hInst, const char *appName,
                                   int width, int height, char const *title)
{
  hMenu_ = NULL;
  hAccel_ = NULL;

  // In VGUI applications, the window size (width, height) passed from
  // vgui::produce_window is usually the image size. Whereas the width
  // and height arguments in function CreateWindow is the window size
  // including UIs like title bar, menu bar, status bar and window frame.
  // Therefore it's better to take account of the dimension of these
  // UIs so that the entire image can be shown.

  // Get the size of UIs
  int cxSizeFrame  = GetSystemMetrics(SM_CXSIZEFRAME);
  int cySizeFrame  = GetSystemMetrics(SM_CYSIZEFRAME);
  int cyCaption    = GetSystemMetrics(SM_CYCAPTION);
  int cyMenu       = GetSystemMetrics(SM_CYMENU);
  int cxScreenSize = GetSystemMetrics(SM_CXFULLSCREEN);
  int cyScreenSize = GetSystemMetrics(SM_CYFULLSCREEN);
  int cxVScroll    = GetSystemMetrics(SM_CXVSCROLL);
  int cyHScroll    = GetSystemMetrics(SM_CYHSCROLL);

  // Compute a reasonable vgui app window size
  int win_width = width+2*cxSizeFrame+3; //+cxVScroll;
  int win_height = height+cyCaption+cyMenu+2*cySizeFrame; //+cyHScroll;

  // But consider the case that image size is larger than the screen size
  if ( win_width > cxScreenSize ) {
    win_width = cxScreenSize;
    cx_ = cxScreenSize-(2*cxSizeFrame);
  }
  if ( win_height > cyScreenSize ) {
    win_height = cyScreenSize;
    cy_ = cyScreenSize-(cyCaption+cyMenu+2*cySizeFrame);
  }

  hwnd_ = CreateWindow(appName,             // window class name
                       title,               // window caption
                       WS_OVERLAPPEDWINDOW, // window style
                       CW_USEDEFAULT,       // initial x position
                       CW_USEDEFAULT,       // initial y position
                       win_width,           // window width
                       win_height,          // window height
                       NULL,                // parent window handle
                       NULL,                // window menu handle
                       hInst,               // program instance handle
                       NULL                 // creation parameters
                      );
  adaptor_ = new vgui_win32_adaptor(hwnd_, this);
  statusbar_ = new vgui_win32_statusbar(hwnd_);
  vgui::out.rdbuf(statusbar_->statusbuf); // redirect to status bar.

  // These two lines may be duplicated as vgui_window::show() has already
  // been called in vgui::adapt(), which is called by vgui::run().
  ShowWindow(hwnd_, SW_SHOWNORMAL);
  UpdateWindow(hwnd_);
}

vgui_win32_window::~vgui_win32_window()
{
  delete statusbar_;
  statusbar_ = 0;

  DestroyMenu(hMenu_);
  hMenu_ = NULL;
  callbacks.clear();

  DestroyAcceleratorTable(hAccel_);
}

// Display the window.
void vgui_win32_window::show()
{
  ShowWindow(hwnd_, SW_SHOW);
  UpdateWindow(hwnd_);

  // These two message are posted so that the embedded adaptor can
  // obtain its size at the application startup.
  // The reason to do so is that the WM_SIZE message and the WM_PAINT
  // message sent at application initialization stage cannot be reached to
  // either vgui_win32_window or vgui_win32_adaptor, which is unborn then.
  // Other ways for the adaptor to get these messages at startup is welcomed.
  if ( first_show ) {
    PostMessage(hwnd_, WM_SIZE, 0, MAKELPARAM((WORD)(cx_+3),(WORD)(cy_) ));
    PostMessage(hwnd_, WM_PAINT, 0, 0);
    first_show = false;
  }
}

// TODO: This function is not called yet.
// Change window shape to new given width and height.
void vgui_win32_window::reshape(unsigned w, unsigned h)
{
   cx_ = w; cy_ = h;
   MoveWindow(hwnd_, wx_, wy_, cx_, cy_, TRUE);
}

// TODO: This function is not called yet.
// Move the window to the new given x,y position.
void vgui_win32_window::reposition(int x, int y)
{
  wx_ = x; wy_ = y;
  MoveWindow(hwnd_, wx_, wy_, cx_, cy_, TRUE);
}


// Put the given menu in the menubar (if it exists).
void vgui_win32_window::set_menubar(vgui_menu const &menu)
{
  // Remove the existent menu handle if it exists.
  if ( hMenu_ ) {
    DestroyMenu(hMenu_);
    hMenu_ = NULL;
  }
  callbacks.clear();

  // Convert vgui_menu to Win32 MENUTEMPLATE to load the new menu.
  hMenu_ = vgui_win32_utils::instance()->vgui_menu_to_win32ex(menu, callbacks, &hAccel_);
  SetMenu(hwnd_, hMenu_);
  // The changed menu won't be updated unless it is forced to be redrawn.
  DrawMenuBar(hwnd_);
}


// If true, activate the statusbar (if it exists).
void vgui_win32_window::set_statusbar(bool on)
{
  if ( statusbar_ )
    statusbar_->setVisible(on);
}


// If true, activate horizontal scrollbar (if it exists).
void vgui_win32_window::enable_hscrollbar(bool b)
{
  EnableScrollBar(hwnd_, SB_HORZ, b ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
  ShowScrollBar(hwnd_, SB_HORZ, b);
}


// If true, activate vertical scrollbar (if it exists).
void vgui_win32_window::enable_vscrollbar(bool b)
{
  EnableScrollBar(hwnd_, SB_VERT, b ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH);
  ShowScrollBar(hwnd_, SB_VERT, b);
}

// Set the position of the horizontal scrollbar, returns old position
int vgui_win32_window::set_hscrollbar(int pos)
{
  SCROLLINFO si;
  int oldPos;

  si.cbSize = sizeof(si);
  si.fMask = SIF_POS;

  GetScrollInfo(hwnd_, SB_HORZ, &si);
  oldPos = si.nPos;

  si.nPos = pos;
  SetScrollInfo(hwnd_, SB_HORZ, &si, TRUE);

  return oldPos;
}

// Set the position of the vertical scrollbar, returns old position
int vgui_win32_window::set_vscrollbar(int pos)
{
  SCROLLINFO si;
  int oldPos;

  si.cbSize = sizeof(si);
  si.fMask = SIF_POS;

  GetScrollInfo(hwnd_, SB_VERT, &si);
  oldPos = si.nPos;

  si.nPos = pos;
  SetScrollInfo(hwnd_, SB_VERT, &si, TRUE);

  return oldPos;
}

void vgui_win32_window::menu_dispatcher(int menuId)
{
  int item_count = callbacks.size();

  // Make sure nID is in the relevant range, and
  // call the callback function associated with the menu item "menuId"
  if ( menuId >= MENU_ID_START && menuId < MENU_ID_START+item_count )
    callbacks[menuId-MENU_ID_START]->execute();
}

// Handle messages that are related to vgui_window.
BOOL vgui_win32_window::OnCmdMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
  if ( adaptor_ )
    adaptor_->OnCmdMsg(message, wParam, lParam);

  switch (message) {
    case WM_SIZE:
      cx_ = LOWORD(lParam);
      cy_ = HIWORD(lParam);
      // redraw status bar
      SendMessage(statusbar_->getWindowHandle(), WM_SIZE, wParam, lParam);
      break;

    case WM_COMMAND: // child window and menu message
      menu_dispatcher(LOWORD(wParam));
      return TRUE;

    case WM_CLOSE:
      delete adaptor_;
      adaptor_ = 0;
      break;

    //case WM_DESTROY:
    //  PostQuitMessage(0);
    //  break;
  }

  return FALSE;
}
