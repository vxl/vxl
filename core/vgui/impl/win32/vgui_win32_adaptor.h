// This is core/vgui/impl/win32/vgui_win32_adaptor.h
#ifndef vgui_win32_adaptor_h_
#define vgui_win32_adaptor_h_
//:
// \file
// \brief The Win32 Application Programming Interfaces (API) implementation of vgui_adaptor.
// \author Lianqing Yu
// \date July 29, 2009              Initial version
//
// \todo TODO issues:
// 1. idle_slot.
// 2. Should the timer be created in the constructor, in case post_timer is
//    never called? Qt impl. does this but MFC impl. does not.
// 3. How to implement post_message?

#include <map>
#include <windows.h>

#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_adaptor_mixin.h>
#include <vgui/impl/win32/vgui_win32_cmdtarget.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vgui_win32_window;
class vgui_win32_internal_timer;

class vgui_win32_adaptor : public vgui_adaptor, public vgui_adaptor_mixin,
                           public vgui_win32_cmdtarget
{
 public:
  typedef vgui_adaptor_mixin mixin;

  vgui_win32_adaptor(HWND hwnd, vgui_window *win = 0);
  ~vgui_win32_adaptor();

  // Set the size of rendering area. This is convinient for tableaus that
  // cannot receive WM_SIZE message.
  void set_width(unsigned w) { width = w; }
  void set_height(unsigned h) { height = h; }

  // Return width of rendering area, NOT the width of the viewport.
  virtual unsigned get_width() const { return width; }

  // Return height of rendering area, NOT the height of the viewport.
  virtual unsigned get_height() const { return height; }

  // These methods are called by vgui_adaptor (in its capacity as a base class)
  // when a post arrives.

  // Create a timer (id) to dispatch WM_TIMER event every tm milliseconds.
  virtual void post_timer(float tm, int id);
  // Redraw the rendering area.
  virtual void post_redraw();
  // Redraw the overlay buffer
  virtual void post_overlay_redraw();
  virtual void post_idle_request();
  //virtual void post_message(char const *, void const *);
  virtual void post_destroy();

  // kill an existing timer
  virtual void kill_timer(int timer);

  // Bind the given modifier/button combination to the popup menu.
  virtual void bind_popups(vgui_modifier m, vgui_button b)
  { mixin::popup_modifier = m; mixin::popup_button = b; }

  // Return the modifier/button which pops up the popup menu.
  virtual void get_popup_bindings(vgui_modifier &m, vgui_button &b) const
  { m = mixin::popup_modifier; b = mixin::popup_button; }


  // getting the window.
  virtual vgui_window *get_window() const { return win_; }

  // various buffer behaviour.
  virtual void swap_buffers() { SwapBuffers(hdc_); }
  virtual void make_current() { wglMakeCurrent(hdc_, hglrc_); }

  // Message handling function
  virtual BOOL OnCmdMsg(UINT message, WPARAM wParam, LPARAM lParam);
  // Called within message processing loop.
  void menu_dispatcher(int menuId);

  // Message callback functions
  void OnCreate();
  //void OnDestroy();
  void OnSize(WPARAM wParam, LPARAM lParam);
  void OnPaint();
  void OnTimer(WPARAM wParam, LPARAM lParam);
  void OnHScroll(UINT message, WPARAM wParam, LPARAM lParam);
  void OnVScroll(UINT message, WPARAM wParam, LPARAM lParam);
  void OnKeyDown(WPARAM wParam, LPARAM lParam);
  void OnKeyUp(WPARAM wParam, LPARAM lParam);
  //void OnChar();
  void OnLButtonDown(WPARAM wParam, LPARAM lParam);
  void OnLButtonUp(WPARAM wParam, LPARAM lParam);
  void OnMButtonDown(WPARAM wParam, LPARAM lParam);
  void OnMButtonUp(WPARAM wParam, LPARAM lParam);
  void OnRButtonDown(WPARAM wParam, LPARAM lParam);
  void OnRButtonUp(WPARAM wParam, LPARAM lParam);
  void OnMouseMove(WPARAM wParam, LPARAM lParam);
  void OnMouseWheel(WPARAM wParam, LPARAM lParam);

 protected:
  // Translate a win32 message into the corresponding VGUI event.
  vgui_event translate_message(WPARAM wParam, LPARAM lParam,
                               vgui_event_type evtype = vgui_EVENT_NULL);
  // Translate a win32 key into the corresponding VGUI key
  void translate_key(UINT nChar, UINT nFlags, int *key, int *ascii_char);
  // Handle mouse event
  void domouse(vgui_event_type t, vgui_button b, UINT nFlags, int x, int y);

  HWND   hwnd_;  // main window handle
  HGLRC  hglrc_; // OpenGL rendering context
  HDC    hdc_;   // device context
  int    tid_;   // timer identifier

  vgui_window *win_; // the window that contains this adaptor

  // map of timers currently in use.
  std::map<unsigned int, vgui_win32_internal_timer> timers_;

  static vgui_menu last_popup;
  std::vector<vgui_command_sptr> popup_callbacks; // commands called by popup menu items

 private:
  HGLRC setup_for_gl(HDC);

  // True while a redraw event has been requested but not implemented.
  bool redraw_posted_;

  // True while a overlay redraw event has been requested but not implemented.
  bool overlay_redraw_posted_;

  // True while an idle time has been requested but not implemented.
  bool idle_request_posted_;

  DECLARE_MESSAGE_MAP()
};

class vgui_win32_internal_timer
{
 public:
  vgui_win32_internal_timer() : timer_id(0), callback_ptr(0) {}
  vgui_win32_internal_timer(unsigned int id, void *p)
  : timer_id(id), callback_ptr(p) {}

  unsigned int timer_id;
  void* callback_ptr;
};

#endif // vgui_win32_adaptor_h_
