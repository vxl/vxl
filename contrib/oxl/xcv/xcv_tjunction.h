#ifndef xcv_tjunction_h_
#define xcv_tjunction_h_

//--------------------------------------------------------------------------------
// .NAME	xcv_tjunction
// .INCLUDE	xcv/xcv_tjunction.h
// .FILE	xcv_tjunction.cxx
// .SECTION Author
//   K.Y.McGaul
// .SECTION Modifications:
//   K.Y.McGaul     05-MAY-2000    Initial version.
//   Marko Bacic   15-AUG-2000    Reversed the order in 'handle'
//--------------------------------------------------------------------------------

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
#include "xcv_mview_manager.h"

class xcv_tjunction : public vgui_tableau
{
public:
  xcv_tjunction(xcv_mview_manager* the_manager);
  ~xcv_tjunction();

  vcl_string type_name() const;
  vcl_string file_name() const;
  vcl_string pretty_name() const;

  void set_child(vgui_tableau_sptr const& new_child);
  bool handle(const vgui_event&);

private:
  xcv_mview_manager* xcv_mgr;
  vgui_slot child_tab;
};

#endif // xcv_tjunction_h_
