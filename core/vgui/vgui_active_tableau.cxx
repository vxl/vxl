// This is core/vgui/vgui_active_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief  See vgui_active_tableau.h for a description of this file.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   25 Jan 00
//
// \verbatim
//  Modifications
//   25-JAN-2000 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_active_tableau.h"

#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_active_tableau_new
vgui_active_tableau::vgui_active_tableau(vgui_tableau_sptr const & t,
                                         bool name_in_menu )
  : vgui_wrapper_tableau(t)
  , active_(true)
  , visible_(true)
  , name_in_menu_( name_in_menu )
{
}

//----------------------------------------------------------------------------
//: Destructor - called by vgui_active_tableau_sptr.
vgui_active_tableau::~vgui_active_tableau()
{
}

//----------------------------------------------------------------------------
//: Returns the type of this tableau ('vgui_active_tableau').
vcl_string vgui_active_tableau::type_name() const
{
  return "vgui_active_tableau";
}


//----------------------------------------------------------------------------
//: Handles all events sent to this tableau.
//  If visible, then use drawing events, else pass them to the child tableau.
//  If active, use non-drawing events, else pass them to the child tableau.
bool vgui_active_tableau::handle(const vgui_event &e)
{
  // If tableau is not visible then do nothing with vgui_DRAW,
  // or vgui_DRAW_OVERLAY
  if (!visible_)
    if (e.type == vgui_DRAW || e.type == vgui_DRAW_OVERLAY)
      return false;

  // If tableau is inactive then ignore all events except draw events.
  if (!active_)
    if (e.type != vgui_DRAW &&e.type != vgui_DRAW_OVERLAY)
      return false;

  return vgui_wrapper_tableau::handle(e);
}


//----------------------------------------------------------------------------
//: Add option to the popup menu to toggle active and visible.
void vgui_active_tableau::add_popup(vgui_menu& menu)
{
  vgui_menu popup;

  vcl_string active_label("Toggle active ");
  if (active_) active_label+="[on]";
  else active_label+="[off]";

  popup.add(active_label.c_str(),
            new vgui_command_simple<vgui_active_tableau>(this, &vgui_active_tableau::toggle_active));


  vcl_string visible_label("Toggle visible ");
  if (visible_) visible_label+="[on]";
  else visible_label+="[off]";

  popup.add(visible_label.c_str(),
            new vgui_command_simple<vgui_active_tableau>(this, &vgui_active_tableau::toggle_visible));

  if (!name_in_menu_)
    menu.include( popup );
  else
    menu.add( name(), popup );
}

//----------------------------------------------------------------------------
//: Toggle between active (using events) and inactive (passing events on).
void vgui_active_tableau::toggle_active()
{
  active_ = !active_;
  post_redraw();
}

//----------------------------------------------------------------------------
//: Toggle between visible (using drawing events) and invisible.
void vgui_active_tableau::toggle_visible()
{
  visible_ = !visible_;
  post_redraw();
}

