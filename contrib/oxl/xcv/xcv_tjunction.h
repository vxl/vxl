// This is oxl/xcv/xcv_tjunction.h
#ifndef xcv_tjunction_h_
#define xcv_tjunction_h_
//:
// \file
// \author  K.Y.McGaul
// \brief Passes events to a tableau and a xcv_mview_manager.
//
//  Events sent to this tableau are passed on to both its child tableau
//  and to the xcv_mview_manager registered in the constructor.
//
// \verbatim
//  Modifications:
//   K.Y.McGaul     05-MAY-2000    Initial version.
//   Marko Bacic    15-AUG-2000    Reversed the order in 'handle'
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include "xcv_mview_manager.h"

//:
//  Events sent to this tableau are passed on to both its child tableau
//  and to the xcv_mview_manager registered in the constructor.

class xcv_tjunction : public vgui_tableau
{
 public:
  xcv_tjunction(xcv_mview_manager* the_manager);
  ~xcv_tjunction();

  vcl_string type_name() const;
  vcl_string file_name() const;
  vcl_string pretty_name() const;

  void set_child(vgui_tableau_sptr const& new_child);
  //: Handle events by passing them to the manager and the child tableau.
  bool handle(const vgui_event&);

 private:
  xcv_mview_manager* xcv_mgr;
  vgui_parent_child_link child_tab;
};

#endif // xcv_tjunction_h_
