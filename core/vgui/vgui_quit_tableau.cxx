// This is oxl/vgui/vgui_quit_tableau.cxx
#include "vgui_quit_tableau.h"
//:
// \file
// \author  fsm
// \brief   See vgui_quit_tableau.h for a description of this file.

#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_adaptor.h>
#include <vcl_iostream.h>

bool vgui_quit_tableau::handle(vgui_event const &e) {
  if (e.type==vgui_KEY_PRESS) {
    if ((e.key=='q' && (e.modifier & vgui_META || e.modifier & vgui_ALT)) || e.key==vgui_ESCAPE) {
      vcl_cerr << __FILE__ " : quit" << vcl_endl;
      e.origin->post_destroy();
      vgui::quit();
      return true;
    } else if (e.key=='w' && (e.modifier & vgui_META || e.modifier & vgui_ALT)) {
      e.origin->post_destroy();
      return true;
    } else
      return false;
  } else
    return false;
}

vcl_string vgui_quit_tableau::type_name() const {
  return "vgui_quit_tableau"; 
}
