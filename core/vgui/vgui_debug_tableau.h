// This is oxl/vgui/vgui_debug_tableau.h
#ifndef vgui_debug_tableau_h_
#define vgui_debug_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// \date   08-11-99
// \brief  Event-printing "tee" tableau.
//
// \verbatim
//  Modifications:
//    991008 AWF Initial version.
//    26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#include "vgui_debug_tableau_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>

//: Event-printing "tee" tableau.
//  A vgui_tableau that prints events and then forwards them to a child.
class vgui_debug_tableau : public vgui_wrapper_tableau
{
 public:
  // Public interface at the top
  vgui_debug_tableau(vgui_tableau_sptr const& child);

  vcl_string type_name() const { return "debug_tableau"; }

  // Protected members and data
 protected:
  ~vgui_debug_tableau();
  bool handle(const vgui_event& e);
  int verbosity;
};

struct vgui_debug_tableau_new : public vgui_debug_tableau_sptr
{
  typedef vgui_debug_tableau_sptr base;
  vgui_debug_tableau_new(vgui_tableau_sptr const& child) : base(new vgui_debug_tableau(child)) { }
};

#endif // vgui_debug_tableau_h_
