// This is oxl/ogui/impl/glX/vgui_glX.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   16 Sep 99
//-----------------------------------------------------------------------------

#include "vgui_glX.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <vul/vul_trace.h>

#include "vgui_glX_adaptor.h"
#include "vgui_glX_window.h"

vgui_glX* vgui_glX::instance_ = 0;

vgui_glX* vgui_glX::instance() {
  vul_trace;
  if (instance_ == 0)
    instance_ = new vgui_glX;
  vul_trace;

  return instance_;
}

vgui_glX::vgui_glX() { }

vcl_string vgui_glX::name() const { return "glX"; }

void vgui_glX::run() {

  while (true) {
    if (XPending(display)) {

      XEvent xev;
      XNextEvent(display, &xev);


      while (XEventsQueued(display, QueuedAfterReading) > 0) {
        XEvent ahead;
        XPeekEvent(display, &ahead);

        if (xev.type==ButtonPress ||
            xev.type==ButtonRelease ||
            xev.type==KeyPress ||
            xev.type==KeyRelease ||
            ahead.type!=xev.type)
          break;
        else
          XNextEvent(display,&xev);
      }


      // check the event against all the registered windows
      // if a match is found then let that window handle the event
      for (unsigned i=0; i<windows.size(); ++i) {
        vgui_glX_window *window = windows[i];
        if (!window) {
          vcl_cerr << "null window.   windows.size() = " << windows.size() << vcl_endl;
          break;
        }

        Window xwindow = window->window;
        if (xwindow == xev.xany.window)
          window->handle(&xev);
      }
    }
  }
}

void vgui_glX::run_one_event() {
  XEvent xev;
  XNextEvent(display, &xev);


  while (XEventsQueued(display, QueuedAfterReading) > 0) {
    XEvent ahead;
    XPeekEvent(display, &ahead);

    if (xev.type==ButtonPress ||
        xev.type==ButtonRelease ||
        xev.type==KeyPress ||
        xev.type==KeyRelease ||
        ahead.type!=xev.type)
      break;
    else
      XNextEvent(display,&xev);
  }


  // check the event against all the registered windows
  // if a match is found then let that window handle the event
  for (unsigned i=0; i<windows.size(); ++i) {

    vgui_glX_window *window = windows[i];
    if (!window) {
      vcl_cerr << "null window.   windows.size() = " << windows.size() << vcl_endl;
      break;
    }

    Window xwindow = window->window;
    if (xwindow == xev.xany.window)
      window->handle(&xev);
  }
}

void vgui_glX::flush() {
}


void vgui_glX::add_event(const vgui_event& /*event*/) {
}


vgui_window* vgui_glX::produce_window(int width, int height, const vgui_menu& /*menubar*/,
                                          const char* title) {
  vcl_cerr << "vgui_glX: menus are not available\n";
  return new vgui_glX_window(width, height, title);
}

vgui_window* vgui_glX::produce_window(int width, int height,
                                          const char* title) {
  return new vgui_glX_window(width, height, title);
}

vgui_dialog_impl* vgui_glX::produce_dialog(const char* /*name*/) {
  vcl_cerr << "vgui_glX: dialogs are not available\n";
  return 0;
}


void vgui_glX::init(int &/*argc*/, char **/*argv*/) {
  display = XOpenDisplay(0);
  vcl_cerr << "display=" << static_cast<void*>(display) << vcl_endl;
  assert(display);
}
