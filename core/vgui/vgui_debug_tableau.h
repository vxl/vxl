#ifndef vgui_debug_tableau_h_
#define vgui_debug_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME vgui_debug_tableau - Event-printing "tee" tableau
// .INCLUDE vgui/vgui_debug_tableau.h
// .FILE vgui_debug_tableau.cxx
// .SECTION Description
//    A vgui_tableau that prints events and then forwards them to
//    a child.
//
// .SECTION Author
//    awf
//
// .SECTION Modifications
//     991008 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vgui_debug_tableau_ref.h"
#include <vgui/vgui_wrapper_tableau.h>

class vgui_debug_tableau : public vgui_wrapper_tableau {
public:
  // Public interface at the top
  vgui_debug_tableau(vgui_tableau_ref const& child);

  vcl_string type_name() const { return "debug_tableau"; }

  // Protected members and data
protected:
  ~vgui_debug_tableau();
  bool handle(const vgui_event& e);
  int verbosity;
};

struct vgui_debug_tableau_new : public vgui_debug_tableau_ref {
  typedef vgui_debug_tableau_ref base;
  vgui_debug_tableau_new(vgui_tableau_ref const& child) : base(new vgui_debug_tableau(child)) { }
};

#endif // vgui_debug_tableau_h_
