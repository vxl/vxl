// This is oxl/vgui/vgui_active_visible.h

//:
// \file   Robotics Research Group, University of Oxford
// \author Philip C. Pritchett
// \date   25 Jan 00
// \brief  

#ifndef vgui_active_visible_h_
#define vgui_active_visible_h_
#ifdef __GNUC__
#pragma interface
#endif

#include "vgui_active_visible_sptr.h"
#include "vgui_wrapper_tableau.h"

//:
//
// \verbatim
//  Modifications:
//    25-JAN-2000 P.Pritchett - Initial version.
// \endverbatim
class vgui_active_visible : public vgui_wrapper_tableau {
public:
  vgui_active_visible(vgui_tableau_sptr const &, bool name_in_menu = false);

  vcl_string type_name() const;

  bool handle(const vgui_event&);
  void add_popup(vgui_menu&);

  // active/visible flags.
  void toggle_active();
  void toggle_visible();
  bool get_active() const { return active_; }
  bool get_visible() const { return visible_; }
  void set_active(bool v) { active_ = v; }
  void set_visible(bool v) { visible_ = v; }

protected:
 ~vgui_active_visible();
  bool active_;
  bool visible_;
  bool name_in_menu_;
};

struct vgui_active_visible_new : public vgui_active_visible_sptr {
  typedef vgui_active_visible_sptr base;
  vgui_active_visible_new(vgui_tableau_sptr const &a, bool name=false)
    : base(new vgui_active_visible(a,name)) { }
};

#endif // vgui_active_visible_h_
