// This is oxl/vgui/internals/vgui_slot_data.h
#ifndef vgui_slot_data_h_
#define vgui_slot_data_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vcl_vector.h>

struct vgui_slot_impl;
struct vgui_slot;
class vgui_tableau;

class vgui_slot_data
{
 private:
  vcl_vector<vgui_tableau*> parents;
  friend struct vgui_slot_impl;
  friend struct vgui_slot;
};

#endif // vgui_slot_data_h_
