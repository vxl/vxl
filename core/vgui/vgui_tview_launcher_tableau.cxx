// This is core/vgui/vgui_tview_launcher_tableau.cxx
//:
// \file
// \brief  See vgui_tview_launcher.h for a description of this file.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   21 Oct 99
//
// \verbatim
//  Modifications
//   04-OCT-2002 K.Y.McGaul - Sort out bug in vgui_event_condition.
//                          - Launch tview in a dialog box instead of window.
// \endverbatim


#include <iostream>
#include "vgui_tview_launcher_tableau.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_tview_tableau.h>
#include <vgui/vgui_dialog.h>

vgui_tview_launcher_tableau::vgui_tview_launcher_tableau()
  : c_graph(vgui_key('g'), vgui_SHIFT) { }

std::string vgui_tview_launcher_tableau::type_name() const
{
  return "vgui_tview_launcher_tableau";
}

bool vgui_tview_launcher_tableau::handle(const vgui_event& e)
{
  if (c_graph(e)) {
    go(e.origin);
    return true;
  }

  return e.type == vgui_DRAW || e.type == vgui_DRAW_OVERLAY;
}

static void launch(const void* t)
{
  // a static_cast may not cast away const.
  void *Pv = const_cast<void*>(t);
  static_cast<vgui_tview_launcher_tableau*>(Pv)->go(vgui_adaptor::current);
}

void vgui_tview_launcher_tableau::get_popup(const vgui_popup_params& /*params*/, vgui_menu &m)
{
  m.add("View tableau hierarchy", launch, this);
}

void vgui_tview_launcher_tableau::go(vgui_adaptor* a)
{
  if (!a) {
    std::cerr << __FILE__ ": a = 0\n";
    return;
  }
  vgui_tview_tableau_new tview(a->get_tableau());
  vgui_viewer2D_tableau_new viewer(tview);

  vgui_dialog tview_dialog("Tableau hierarchy");
  tview_dialog.inline_tableau(viewer, 300,300);
  tview_dialog.set_ok_button("close");
  tview_dialog.set_cancel_button(nullptr);
  tview_dialog.ask();
  this->post_redraw();
}
