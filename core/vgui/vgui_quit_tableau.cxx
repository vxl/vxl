// This is core/vgui/vgui_quit_tableau.cxx
#include <iostream>
#include "vgui_quit_tableau.h"
//:
// \file
// \author  fsm
// \brief   See vgui_quit_tableau.h for a description of this file.

#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_adaptor.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool vgui_quit_tableau::handle(vgui_event const &e)
{
  if (e.type==vgui_KEY_PRESS) {
    if ((e.key=='q' && (e.modifier & vgui_META || e.modifier & vgui_ALT)) || e.key==vgui_ESCAPE) {
      std::cerr << __FILE__ " : quit\n";
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

std::string vgui_quit_tableau::type_name() const
{
  return "vgui_quit_tableau";
}
