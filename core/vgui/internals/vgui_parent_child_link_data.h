// This is oxl/vgui/internals/vgui_parent_child_link_data.h
#ifndef vgui_parent_child_link_data_h_
#define vgui_parent_child_link_data_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \brief  Cache of parents of a tableau (makes posting redraws more efficient).

#include <vcl_vector.h>

struct vgui_parent_child_link_impl;
struct vgui_parent_child_link;
class vgui_tableau;

//: Cache of parents of a tableau (makes posting redraws more efficient).
class vgui_parent_child_link_data
{
 private:
  vcl_vector<vgui_tableau*> parents;
  friend struct vgui_parent_child_link_impl;
  friend struct vgui_parent_child_link;
};

#endif // vgui_parent_child_link_data_h_
