// This is core/vgui/internals/vgui_parent_child_link_data.h
#ifndef vgui_parent_child_link_data_h_
#define vgui_parent_child_link_data_h_
//:
// \file
// \author fsm
// \brief  Cache of parents of a tableau (makes posting redraws more efficient).
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcl_vector.h>

struct vgui_parent_child_link_impl;
struct vgui_parent_child_link;
class vgui_tableau;

//: Cache of parents of a tableau (makes posting redraws more efficient).
class vgui_parent_child_link_data
{
  vcl_vector<vgui_tableau*> parents;
  friend struct vgui_parent_child_link_impl;
  friend struct vgui_parent_child_link;
};

#endif // vgui_parent_child_link_data_h_
