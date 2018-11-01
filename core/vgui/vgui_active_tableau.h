// This is core/vgui/vgui_active_tableau.h
#ifndef vgui_active_tableau_h_
#define vgui_active_tableau_h_
//:
// \file
// \brief  Tableau which toggles between visible/invisible and active/inactive.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   25 Jan 00
//
//  Contains classes  vgui_active_tableau  vgui_active_tableau_new
//
// \verbatim
//  Modifications
//   25-JAN-2000 P.Pritchett - Initial version.
//   07-AUG-2002 K.Y.McGaul - Changed to and added Doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_active_visible to vgui_active_tableau.
// \endverbatim

#include "vgui_active_tableau_sptr.h"
#include "vgui_wrapper_tableau.h"

//: Tableau which can toggle between visible/invisible and active/inactive.
//
//  By calling toggle_active and toggle_visible this tableau
//  (or rather a tableau derived from it) can appear visible or
//  invisible, and active or inactive.
//
//  Functions to toggle these can appear on the popup menu for the tableau.
//
//  When the tableau is invisible, all vgui_DRAW and vgui_DRAW_OVERLAY
//  mevents are passed to the child tableau.
//
//  When the tableau is inactive, all events apart from draw events
//  are passed directly to the child tableau.
class vgui_active_tableau : public vgui_wrapper_tableau
{
 public:
  //: Constructor - don't use this, use vgui_active_tableau_new.
  vgui_active_tableau(vgui_tableau_sptr const &, bool name_in_menu = false);

  //: Return the name of this tableau ('vgui_active_tableau').
  std::string type_name() const;

  //: Handle all events sent to this tableau.
  //  If visible, then use drawing events, else pass them to the child tableau.
  //  If active, use non-drawing events, else pass them to the child tableau.
  bool handle(const vgui_event&);

  //: Add option to the popup menu to toggle active and visible.
  void add_popup(vgui_menu&);

  //: Toggle between active (using events) and inactive (passing events on).
  void toggle_active();

  //: Toggle between visible (using drawing events) and invisible.
  void toggle_visible();

  //: Return true if the tableau is active.
  bool get_active() const { return active_; }

  //: Return true if the tableau is visible.
  bool get_visible() const { return visible_; }

  //: True makes the tableau active, false makes it inactive.
  void set_active(bool v) { active_ = v; }

  //: True makes the tableau visible, false makes it invisible.
  void set_visible(bool v) { visible_ = v; }

 protected:
 ~vgui_active_tableau();
  bool active_;
  bool visible_;
  bool name_in_menu_;
};

//: Create a smart-pointer to a vgui_active_tableau tableau.
struct vgui_active_tableau_new : public vgui_active_tableau_sptr
{
  typedef vgui_active_tableau_sptr base;

  //: Constructor - make a tableau with the given child and name.
  vgui_active_tableau_new(vgui_tableau_sptr const &a, bool name=false)
    : base(new vgui_active_tableau(a,name)) { }
};

#endif // vgui_active_tableau_h_
