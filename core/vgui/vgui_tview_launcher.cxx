// This is oxl/vgui/vgui_tview_launcher.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 21 Oct 99
//
//-----------------------------------------------------------------------------

#include "vgui_tview_launcher.h"
#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_tview.h>

vgui_tview_launcher::vgui_tview_launcher()
  : c_graph(vgui_key('G')) { }

vcl_string vgui_tview_launcher::type_name() const {
  return "vgui_tview_launcher";
}

bool vgui_tview_launcher::handle(const vgui_event& e) {

  if (c_graph(e)) {
    go(e.origin);
    return true;
  }

  if (e.type == vgui_DRAW || e.type == vgui_DRAW_OVERLAY)
    return true;

  return false;
}

static void launch(const void* t) {
  // a static_cast may not cast away const.
  void *Pv = const_cast<void*>(t);
  static_cast<vgui_tview_launcher*>(Pv)->go(vgui_adaptor::current);
}

void vgui_tview_launcher::get_popup(const vgui_popup_params& /*params*/, vgui_menu &m) {
  m.add("View tableau hierarchy", launch, this);
}

void vgui_tview_launcher::go(vgui_adaptor* a) {
  if (!a) {
    vcl_cerr << __FILE__ ": a = 0\n";
    return;
  }
  vgui_tview_new tview(a->get_tableau());
  vgui_viewer2D_new viewer(tview);
  vgui_window *popup = vgui::produce_window(200,200,"vgui Graph");
  if (popup) {
    popup->set_statusbar(false);
    popup->get_adaptor()->set_tableau(viewer);
    popup->show();
  }
  else {
    vcl_cerr << "vgui_tview_launcher::handle()  Could not popup another vgui window\n";
  }
}
