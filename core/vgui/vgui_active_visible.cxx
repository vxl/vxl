// This is oxl/vgui/vgui_active_visible.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   25 Jan 00
// \brief  See vgui_active_visible.h for a description of this file.
//
// \verbatim
//  Modifications:
//    25-JAN-2000 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_active_visible.h"

#include "vgui_command.h"
#include "vgui_menu.h"

vgui_active_visible::vgui_active_visible(vgui_tableau_sptr const & t, bool name_in_menu )
  : vgui_wrapper_tableau(t)
  , active_(true)
  , visible_(true)
  , name_in_menu_( name_in_menu )
{
}

vgui_active_visible::~vgui_active_visible()
{
}

vcl_string vgui_active_visible::type_name() const {return "vgui_active_visible";}


bool vgui_active_visible::handle(const vgui_event &e) {
  // if tableau is not visible then do nothing with vgui_DRAW, vgui_CLEAR, vgui_DRAW_OVERLAY
  if (!visible_)
    if (e.type == vgui_DRAW || e.type == vgui_DRAW_OVERLAY)
      return false;

  if (!active_)
    if (e.type != vgui_DRAW &&e.type != vgui_DRAW_OVERLAY)
      return false;

  return vgui_wrapper_tableau::handle(e);
}


void vgui_active_visible::add_popup(vgui_menu& menu) {

  vgui_menu popup;

  vcl_string active_label("Toggle active ");
  if (active_) active_label+="[on]";
  else active_label+="[off]";

  popup.add(active_label.c_str(),
            new vgui_command_simple<vgui_active_visible>(this, &vgui_active_visible::toggle_active));


  vcl_string visible_label("Toggle visible ");
  if (visible_) visible_label+="[on]";
  else visible_label+="[off]";

  popup.add(visible_label.c_str(),
            new vgui_command_simple<vgui_active_visible>(this, &vgui_active_visible::toggle_visible));

  if (!name_in_menu_) {
    menu.include( popup );
  } else {
    menu.add( name(), popup );
  }
}


void vgui_active_visible::toggle_active() {
  active_ = !active_;
  post_redraw();
}

void vgui_active_visible::toggle_visible() {
  visible_ = !visible_;
  post_redraw();
}

