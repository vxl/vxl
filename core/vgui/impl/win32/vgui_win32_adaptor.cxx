// This is core/vgui/impl/win32/vgui_win32_adaptor.cxx
#include <iostream>
#include <cstring>
#include <utility>
#include "vgui_win32_adaptor.h"
#include "vgui_win32_utils.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_find.h> // for vgui_find_below_by_type_name
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_command.h>

vgui_menu vgui_win32_adaptor::last_popup;

BEGIN_MESSAGE_MAP(vgui_win32_adaptor, vgui_win32_cmdtarget)
END_MESSAGE_MAP()

vgui_win32_adaptor::vgui_win32_adaptor(HWND hwnd, vgui_window *win)
  : hwnd_(hwnd), win_(win), tid_(0),
  redraw_posted_(true),
  overlay_redraw_posted_(true),
  idle_request_posted_(false)
{
  // Get the device context for the client area of the specified window hwnd.
  hdc_ = GetDC(hwnd);

  // Set up OpenGL environment
  hglrc_ = setup_for_gl(hdc_);
}

vgui_win32_adaptor::~vgui_win32_adaptor()
{
  // Although the vgui_DESTROY event is not handled in vgui,
  // it is (will be) processed by some vxl applications.
  // An examples is example_multiple_windows.
  // Note: dispatch the vgui_DESTROY event to tableau hierarchy
  // before deleting the rendering context, otherwise
  // vgui_projection_inspector::inspect() would fail.
  dispatch_to_tableau(vgui_DESTROY);

  if ( hglrc_ ) {
    // make the rendering context not current
    wglMakeCurrent( NULL, NULL );
    // delete the rendering context
    wglDeleteContext(hglrc_);
  }
  ReleaseDC(hwnd_, hdc_);

  // Clear pointers to context-menu callback functions
  popup_callbacks.clear();

  // Kill the timer
  if ( tid_ )
    kill_timer(tid_);
}

void vgui_win32_adaptor::post_timer(float timeout, int name)
{
  std::map<unsigned int, vgui_win32_internal_timer>::iterator it = timers_.find(name);
  if ( it == timers_.end() ) { // Create at timer if it does rxist
    unsigned int tid = SetTimer(hwnd_, name, (unsigned int)timeout, NULL);
    if ( tid ) { // function fails to create a timer if tid==0
      vgui_win32_internal_timer it(tid, NULL);
      timers_.insert(std::pair<unsigned int, vgui_win32_internal_timer>(name, it));
    }
  }
}


// Redraw the rendering area.
void vgui_win32_adaptor::post_redraw()
{
  if ( !redraw_posted_ ) {
    RECT rect;
    if ( GetUpdateRect(hwnd_, &rect, FALSE) )
      InvalidateRect(hwnd_, &rect, FALSE); // update a region if possible
    else
      InvalidateRect(hwnd_, NULL, FALSE);  // entire client area
    // Redraw scroll bars to reflect to changed image size.
    // This operation might be put in vgui_view2D_tableau.
    PostMessage(hwnd_, WM_SIZE, 0, MAKELPARAM(width, height));
  }
  redraw_posted_ = true;
}

// Redraw overlay buffer
void vgui_win32_adaptor::post_overlay_redraw()
{
  if ( !overlay_redraw_posted_ ) {
    RECT rect;
    if ( GetUpdateRect(hwnd_, &rect, FALSE) )
      InvalidateRect(hwnd_, &rect, FALSE); // update a region if possible
    else
      InvalidateRect(hwnd_, NULL, FALSE);  // entire client area
  }
  overlay_redraw_posted_ = true;
}

// TODO: This function is not called yet.
void vgui_win32_adaptor::post_idle_request()
{
  idle_request_posted_ = true;
}

//void post_message(char const *, void const *);

// Schedules destruction of parent vgui_window.
void vgui_win32_adaptor::post_destroy()
{
  // Do not send WM_DESTROY so as to give parent vgui_window
  // a chance to give focus to other vgui_window(s).
  // See vgui/examples/example_multiple_windows.
  PostMessage(hwnd_, WM_CLOSE, 0, 0);
}

// kill an existing timer
void vgui_win32_adaptor::kill_timer(int name)
{
  std::map<unsigned int, vgui_win32_internal_timer>::iterator it = timers_.find(name);
  if ( it == timers_.end() ) // return if such a timer does not exist
    return;

  KillTimer(hwnd_, it->second.timer_id);
  timers_.erase(it);
}


HGLRC vgui_win32_adaptor::setup_for_gl(HDC hdc)
{
  // Set up pixel format.
  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    24,
    0, 0, 0, 0, 0, 0,
    0, 0,
    0, 0, 0, 0, 0,
    16,                 // 16-bit depth buffer
    0,                  // no stencil buffer
    0,                  // no aux buffers
    PFD_MAIN_PLANE,     // main layer
    0,
    0, 0, 0
  };

  int  selected_pf = ChoosePixelFormat(hdc, &pfd);
  if ( selected_pf == 0 ) {
    MessageBox(NULL, TEXT("Failed to ChoosePixelFormat"), TEXT("Error"),
               MB_ICONERROR | MB_OK);
    std::cerr << "Failed to ChoosePixelFormat (error code:" << GetLastError() << ")\n";
    return 0;
  }

  if ( !SetPixelFormat(hdc, selected_pf, &pfd) ) {
    MessageBox(NULL, TEXT("Failed to SetPixelFormat"), TEXT("Error"),
               MB_ICONERROR | MB_OK);
    std::cerr << "Failed to SetPixelFormat (error code:" << GetLastError() << ")\n";
    return 0;
  }

  // Create a OpenGL rendering context, which is suitable for drawing
  // on the device referenced by hdc_.
  HGLRC hglrc = wglCreateContext(hdc);
  if ( !hglrc ) {
    MessageBox(NULL, TEXT("wglCreateContext failed"), TEXT("Error"),
               MB_ICONERROR | MB_OK);
    std::cerr << "wglCreateContext failed (error code:" << GetLastError() << ")\n";
    return 0;
  }

  // Make all subsequent OpenGL calls to be drawn on the device context hdc_.
  if ( !wglMakeCurrent(hdc, hglrc) ) {
    MessageBox(NULL, TEXT("wglMakeCurrent failed"), TEXT("Error"),
               MB_ICONERROR | MB_OK);
    std::cerr << "wglMakeCurrent failed (error code:" << GetLastError() << ")\n";
    return 0;
  }

  // Specify black as clear color
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // Sepcify the back of the buffer as clear depth
  glClearDepth(1.0f);

  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_BLEND);
  //glShadeModel(GL_SMOOTH);

  return hglrc;
}

// Handling messages that is related to vgui_adaptor.
BOOL vgui_win32_adaptor::OnCmdMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
  switch ( message )
  {
    case WM_SIZE:
      OnSize(wParam, lParam);
      break;
    case WM_PAINT:
      OnPaint();
     break;
    case WM_TIMER:
      OnTimer(wParam, lParam);
      break;
    case WM_HSCROLL:
      OnHScroll(message, wParam, lParam);
      break;
    case WM_VSCROLL:
      OnVScroll(message, wParam, lParam);
      break;
    case WM_KEYDOWN:
      OnKeyDown(wParam, lParam);
      //std::cerr << "tableau: message WM_KEYDOWN received.\n";
      return TRUE;
    case WM_KEYUP:
      OnKeyUp(wParam, lParam);
      //std::cerr << "tableau: message WM_KEYUP received.\n";
      return TRUE;
    case WM_LBUTTONDOWN:
      OnLButtonDown(wParam, lParam);
      return TRUE;
    case WM_MBUTTONDOWN:
      OnMButtonDown(wParam, lParam);
      return TRUE;
    case WM_RBUTTONDOWN:
      OnRButtonDown(wParam, lParam);
      return TRUE;
    case WM_LBUTTONUP:
      OnLButtonUp(wParam, lParam);
      return TRUE;
    case WM_MBUTTONUP:
      OnMButtonUp(wParam, lParam);
      return TRUE;
    case WM_RBUTTONUP:
      OnRButtonUp(wParam, lParam);
      return TRUE;
    case WM_MOUSEMOVE:
      OnMouseMove(wParam, lParam);
      break;
#ifdef WM_MOUSEWHEEL
    case WM_MOUSEWHEEL:
      OnMouseWheel(wParam, lParam);
      //std::cerr << "tableau: message WM_MOUSEWHEEL received.\n";
      return TRUE;
#endif
    case WM_COMMAND: // child window and menu message
      menu_dispatcher(LOWORD(wParam));
      return TRUE;
    default:
      break;
  }

  return FALSE;
}

void vgui_win32_adaptor::menu_dispatcher(int menuId)
{
  int item_count = popup_callbacks.size();

  // Make sure nID is in the relevant range
  if ( menuId >= POPUPMENU_ID_START && menuId < POPUPMENU_ID_START+item_count )
    // Call the callback function associated with the menu item "menuId"
    popup_callbacks[menuId-POPUPMENU_ID_START]->execute();
}

void vgui_win32_adaptor::OnSize(WPARAM wParam, LPARAM lParam)
{
  // Get size of rendering area
  width  = LOWORD(lParam);
  height = HIWORD(lParam);

  // Resize scrollbars according to the window size and rendered image size
  // that take into account of zooming.
  vgui_tableau_sptr imtab = vgui_find_below_by_type_name(
                              this->get_tableau(), "vgui_image_tableau");
  vgui_tableau_sptr vrtab = vgui_find_below_by_type_name(
                              this->get_tableau(), "vgui_viewer2D_tableau");
  if ( imtab && vrtab ) {
    int im_width, im_height;

    vgui_image_tableau_sptr im;
    im.vertical_cast(imtab);
    vgui_viewer2D_tableau_sptr vr;
    vr.vertical_cast(vrtab);

    im_width  = im->width() *vr->token.scaleX;
    im_height = im->height()*vr->token.scaleY;

    SCROLLINFO si;
    // Save vertical scroll bar range and page size
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = 100;
    si.nPage = 100.*width/im_width;
    si.nPos = -100.*vr->token.offsetX/width;
    SetScrollInfo(hwnd_, SB_HORZ, &si, TRUE);

    // Save horizontal scroll bar range and page size
    si.nPage = 100.*height/im_height;
    si.nPos = -100.*vr->token.offsetY/height;
    SetScrollInfo(hwnd_, SB_VERT, &si, TRUE);
  }

  dispatch_to_tableau(vgui_RESHAPE);
  post_redraw();
}

void vgui_win32_adaptor::OnPaint()
{
  if ( redraw_posted_ ) {
    this->make_current();
    vgui_macro_report_errors;
    dispatch_to_tableau(vgui_event(vgui_DRAW));
    vgui_macro_report_errors;
    redraw_posted_ = false;
  }

  if ( overlay_redraw_posted_ ) {
    this->make_current();
    vgui_macro_report_errors;
    // The following line forces a redraw to erase the previous overlay
    dispatch_to_tableau(vgui_event(vgui_DRAW));
    dispatch_to_tableau(vgui_event(vgui_DRAW_OVERLAY));
    vgui_macro_report_errors;
    overlay_redraw_posted_ = false;
  }

  swap_buffers();
}

void vgui_win32_adaptor::OnTimer(WPARAM wParam, LPARAM lParam)
{
  vgui_event e(vgui_TIMER);
  e.timer_id = wParam;
  dispatch_to_tableau(e);
}

void vgui_win32_adaptor::OnHScroll(UINT message, WPARAM wParam, LPARAM lParam)
{
  static SCROLLINFO si;
  int iHorzPos;
  vgui_event e;

  // Get all the vertical scroll bar information
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
  GetScrollInfo(hwnd_, SB_HORZ, &si);

  // Save the position for comparison later on
  iHorzPos = si.nPos;
  switch (LOWORD(wParam)) {
    case SB_LINELEFT:
      si.nPos -= 1;
      break;
    case SB_LINERIGHT:
      si.nPos += 1;
      break;
    case SB_PAGELEFT:
      si.nPos -= si.nPage;
      break;
    case SB_PAGERIGHT:
      si.nPos += si.nPage;
      break;
    case SB_THUMBPOSITION:
      si.nPos = si.nTrackPos;
      break;
    default:
      break;
  }

  // Set the position and then retrieve it. Due to adjustments
  // by Windows it may not be the same as the value set.
  si.fMask = SIF_POS;
  SetScrollInfo(hwnd_, SB_HORZ, &si, TRUE);
  GetScrollInfo(hwnd_, SB_HORZ, &si);

  // If the position has changed, scroll the window and update it
  if ( si.nPos != iHorzPos ) {
    e.type = vgui_HSCROLL;
    e.data = &si.nPos;
    dispatch_to_tableau(e);

    ScrollWindow(hwnd_, 0, (iHorzPos-si.nPos), NULL, NULL);
    UpdateWindow(hwnd_);
  }
}

void vgui_win32_adaptor::OnVScroll(UINT message, WPARAM wParam, LPARAM lParam)
{
  static SCROLLINFO si;
  int iVertPos;
  vgui_event e;

  // Get all the vertical scroll bar information
  si.cbSize = sizeof(si);
  si.fMask = SIF_ALL;
  GetScrollInfo(hwnd_, SB_VERT, &si);

  // Save the position for comparison later on
  iVertPos = si.nPos;
  switch (LOWORD(wParam)) {
    case SB_TOP:
      si.nPos = si.nMin;
      break;
    case SB_BOTTOM:
      si.nPos = si.nMax;
      break;
    case SB_LINEUP:
      si.nPos -= 1;
      break;
    case SB_LINEDOWN:
      si.nPos += 1;
      break;
    case SB_PAGEUP:
      si.nPos -= si.nPage;
      break;
    case SB_PAGEDOWN:
      si.nPos += si.nPage;
      break;
    case SB_THUMBTRACK:
      si.nPos = si.nTrackPos;
      break;
    default:
      break;
  }

  // Set the position and then retrieve it. Due to adjustments
  // by Windows it may not be the same as the value set.
  si.fMask = SIF_POS;
  SetScrollInfo(hwnd_, SB_VERT, &si, TRUE);
  GetScrollInfo(hwnd_, SB_VERT, &si);

  // If the position has changed, scroll the window and update it
  if ( si.nPos != iVertPos ) {
    e.type = vgui_VSCROLL;
    e.data = &si.nPos;
    dispatch_to_tableau(e);

    ScrollWindow(hwnd_, 0, (iVertPos-si.nPos), NULL, NULL);
    UpdateWindow(hwnd_);
  }
}

void vgui_win32_adaptor::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
  // Ignore Ctrl and Shift pressed alone.
  if ( wParam == VK_SHIFT || wParam == VK_CONTROL )
    return;

  vgui_event ev = translate_message(wParam, lParam, vgui_KEY_PRESS);
  dispatch_to_tableau(ev);
}

void vgui_win32_adaptor::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
  // Ignore Ctrl and Shift pressed alone.
  if ( wParam == VK_SHIFT || wParam == VK_CONTROL )
    return;

  vgui_event ev = translate_message(wParam, lParam, vgui_KEY_UP);
  dispatch_to_tableau(ev);
}

void vgui_win32_adaptor::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
  domouse(vgui_BUTTON_DOWN, vgui_LEFT, wParam, LOWORD(lParam), HIWORD(lParam));
}

void vgui_win32_adaptor::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
  domouse(vgui_BUTTON_UP, vgui_LEFT, wParam, LOWORD(lParam), HIWORD(lParam));
}

void vgui_win32_adaptor::OnMButtonDown(WPARAM wParam, LPARAM lParam)
{
  domouse(vgui_BUTTON_DOWN, vgui_MIDDLE, wParam, LOWORD(lParam), HIWORD(lParam));
}

void vgui_win32_adaptor::OnMButtonUp(WPARAM wParam, LPARAM lParam)
{
  domouse(vgui_BUTTON_UP, vgui_MIDDLE, wParam, LOWORD(lParam), HIWORD(lParam));
}

void vgui_win32_adaptor::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
  domouse(vgui_BUTTON_DOWN, vgui_RIGHT, wParam, LOWORD(lParam), HIWORD(lParam));
}

void vgui_win32_adaptor::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
  domouse(vgui_BUTTON_UP, vgui_RIGHT, wParam, LOWORD(lParam), HIWORD(lParam));
}


void vgui_win32_adaptor::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
  domouse(vgui_MOTION, vgui_BUTTON_NULL, wParam, LOWORD(lParam), HIWORD(lParam));
}

void vgui_win32_adaptor::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
  short delta = HIWORD(wParam);
  domouse( delta > 0 ? vgui_WHEEL_UP : vgui_WHEEL_DOWN,
           vgui_BUTTON_NULL, LOWORD(wParam), LOWORD(lParam), HIWORD(lParam));
}

// Translate a win32 key into the corresponding VGUI key
void vgui_win32_adaptor::translate_key(UINT nChar, UINT nFlags,
                                       int *the_key, int *the_ascii_char)
{
  if (nFlags & 256) { // The 8-th bit is the extended key
    // Extended key, such as a function key or a key on hte numeric keypad.
    switch (nChar) {
      case VK_NEXT:
        *the_key = vgui_PAGE_DOWN;
        *the_ascii_char = vgui_PAGE_DOWN;
        return;
      case VK_PRIOR:
        *the_key = vgui_PAGE_UP;
        *the_ascii_char = vgui_PAGE_UP;
        return;
      case VK_HOME:
        *the_key = vgui_HOME;
        *the_ascii_char = vgui_HOME;
        return;
      case VK_END:
        *the_key = vgui_END;
        *the_ascii_char = vgui_END;
        return;
      case VK_LEFT:
        *the_key = vgui_CURSOR_LEFT;
        *the_ascii_char = vgui_CURSOR_LEFT;
        return;
      case VK_UP:
        *the_key = vgui_CURSOR_UP;
        *the_ascii_char = vgui_CURSOR_UP;
        return;
      case VK_RIGHT:
        *the_key = vgui_CURSOR_RIGHT;
        *the_ascii_char = vgui_CURSOR_RIGHT;
        return;
      case VK_DOWN:
        *the_key = vgui_CURSOR_DOWN;
        *the_ascii_char = vgui_CURSOR_DOWN;
        return;
      case VK_DELETE:
        *the_key = vgui_DELETE;
        *the_ascii_char = vgui_DELETE;
        return;
      case VK_INSERT:
        *the_key = vgui_INSERT;
        *the_ascii_char = vgui_INSERT;
        return;

      default:
        *the_key = vgui_key(0);
        *the_ascii_char = vgui_key(0);
        return;
    }
  }
  else if (nChar >= VK_F1 && nChar <= VK_F12)
  {
    *the_key = *the_ascii_char = vgui_F1 + (nChar - VK_F1);
  }
  else if (nChar == VK_ESCAPE )
    *the_key = *the_ascii_char = vgui_ESC;
  else if (nChar == VK_TAB )
    *the_key = *the_ascii_char = vgui_TAB;
  else if (nChar == VK_RETURN )
    *the_key = *the_ascii_char = vgui_RETURN;
  else
  {
    unsigned short buf[1024];
    unsigned char lpKeyState[256];
    std::memset(lpKeyState, 0, 256);
    std::memset(buf, 0, 256);

    int is_ok = ToAscii(nChar, nFlags & 0xff, lpKeyState, buf, 0);
    if (is_ok == 1)
      *the_key = buf[0];
    else
      *the_key = nChar;

    // Add modifiers to character:
    lpKeyState[VK_SHIFT] = GetKeyState(VK_SHIFT);
    lpKeyState[VK_CONTROL] = GetKeyState(VK_CONTROL);

    is_ok = ToAscii(nChar, nFlags & 0xff, lpKeyState, buf, 0);
    if (is_ok == 1)
      *the_ascii_char = buf[0];
    else
      *the_ascii_char = nChar;
  }
}

// Translate a Windows message into corresponding vgui event.
vgui_event vgui_win32_adaptor::translate_message(WPARAM wParam,
                                                 LPARAM lParam, vgui_event_type evtype)
{
  vgui_event ev(evtype);

  // Determine modifier.
  if ( GetKeyState(VK_SHIFT) & 0x8000 )
    ev.modifier = vgui_SHIFT;
  if ( GetKeyState(VK_CONTROL) & 0x8000 )
    ev.modifier = vgui_CTRL;
  if ( GetKeyState(VK_MENU) & 0x8000 )
    ev.modifier = vgui_ALT;

  // Get key and ASCII character
  int nChar, nFlags, key, ascii_char;
  nChar = wParam; // nChar and nFlags corresponds to the arguments of
  nFlags = HIWORD(lParam); // CWnd::OnKeyDown().
  translate_key(nChar, nFlags, &key, &ascii_char);
  ev.set_key(vgui_key(key));
  ev.ascii_char = vgui_key(ascii_char);

  return ev;
}

void vgui_win32_adaptor::domouse(vgui_event_type et, vgui_button b, UINT nFlags, int x, int y)
{
  // awf: BLETCH. This offset is consistent over resize, depth, screen position, machines,
  // and I can't find it... Sorry.
  x += 2;
  y += 2;
  // FIXME

  vgui_event e(et);

  e.button = b;

  // Double-check the button. This op may correct the wrong calling argument.
  if (nFlags & MK_LBUTTON)
    e.button = vgui_LEFT;
  if (nFlags & MK_MBUTTON)
    e.button = vgui_MIDDLE;
  if (nFlags & MK_RBUTTON)
    e.button = vgui_RIGHT;

  if (nFlags & MK_SHIFT)
    e.modifier = vgui_modifier((int)e.modifier | vgui_SHIFT);
  if (nFlags & MK_CONTROL)
    e.modifier = vgui_modifier((int)e.modifier | vgui_CTRL);

  e.wx = x;
  e.wy = height - y;

  if (e.modifier == mixin::popup_modifier  && e.button == mixin::popup_button
                                           && e.type == vgui_BUTTON_UP)
  {
    vgui_popup_params params;
    params.x = x;
    params.y = y;
    last_popup = get_total_popup(params);


    HMENU hMenu = vgui_win32_utils::instance()->vgui_menu_to_win32ex(
                  last_popup, popup_callbacks, 0, true);

    POINT point;
    point.x = x; point.y = y;
    // TrackPopupMenu requires screen coordinates whereas (x,y) is
    // client coordinates.
    ClientToScreen(hwnd_, &point);
    HMENU hPopupMenu = GetSubMenu(hMenu, 0);
    TrackPopupMenu(hPopupMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd_, NULL);
  }
  else
    dispatch_to_tableau(e);

# if 0 // explained below:
  // Grabbing the mouse here causes an issue with code that runs
  // another instance of the event loop in response to the event (sent
  // by the dispatch_to_tableau call above).  An example if
  // vgui_rubberband_tableau, which, on a point add, could cause a
  // dialog to pop up.  In general, grabbing the mouse is a very
  // client specific thing, and we should probably not be doing it all
  // the time, as it is being done here.  If we want, we may consider
  // putting this in the interface.  However, given vgui's goals of
  // being a light & thin wrapper, I don't think that's a good idea.

   // Grab mouse?
   {
     if (et == vgui_BUTTON_DOWN) {
       SetCapture(hwnd_);
     }
     else if (et != vgui_MOTION) {
       ReleaseCapture();
     }
   }
#endif // 0
}
