// This is oxl/vgui/impl/Xm/vgui_Xm.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   16 Sep 99
//-----------------------------------------------------------------------------

#include "vgui_Xm.h"

#include <vcl_algorithm.h>

#include "vgui_Xm_window.h"
#include "vgui_Xm_dialog_impl.h"

#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>

static bool debug=false;

vgui_Xm* vgui_Xm::instance_ = 0;

static vgui_Xm* vgui_Xm_instantiation = vgui_Xm::instance();

vgui_Xm* vgui_Xm::instance()
{
  if (instance_ == 0)
    instance_ = new vgui_Xm;

  return instance_;
}

vgui_Xm::vgui_Xm()
{
}

vcl_string vgui_Xm::name() const { return "motif"; }

void vgui_Xm::run()
{
  XtAppMainLoop(app_context);
}

void vgui_Xm::run_one_event()
{
  XEvent xevent;
  XtAppNextEvent(app_context, &xevent);
  XtDispatchEvent(&xevent);
}

vgui_window* vgui_Xm::produce_window(int width, int height, const vgui_menu& menubar,
                                          const char* title)
{
  if (debug) vcl_cerr << "vgui_Xm::produce_window_impl\n";
  return new vgui_Xm_window(width, height, menubar, title);
}

vgui_window* vgui_Xm::produce_window(int width, int height,
                                          const char* title)
{
  if (debug) vcl_cerr << "vgui_Xm::produce_window_impl\n";
  return new vgui_Xm_window(width, height, title);
}

vgui_dialog_impl* vgui_Xm::produce_dialog(const char* name)
{
  return new vgui_Xm_dialog_impl(name);
}


static String fallbackResources[] = {NULL};


void vgui_Xm::init(int &argc, char **argv)
{
  app_widget = XtAppInitialize(&app_context, "vgui", NULL, 0,
                               &argc, argv, fallbackResources, NULL, 0);
}

