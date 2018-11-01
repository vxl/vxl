// This is core/vgui/vgui_debug_tableau.h
#ifndef vgui_debug_tableau_h_
#define vgui_debug_tableau_h_
//:
// \file
// \brief  Event-printing "tee" tableau.
// \author awf@robots.ox.ac.uk
// \date   08-Nov-1999
//
//  Contains classes  vgui_debug_tableau  vgui_debug_tableau_new
//
// \verbatim
//  Modifications
//   1999-10-08 AWF Initial version.
//   26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#include "vgui_debug_tableau_sptr.h"
#include <vgui/vgui_wrapper_tableau.h>

//: Event-printing "tee" tableau.
//
//  A vgui_tableau that prints events and then forwards them to a child.
class vgui_debug_tableau : public vgui_wrapper_tableau
{
 public:
  //: Constructor - don't use this, use vgui_debug_tableau_new.
  //  Takes the one and only child of this tableau.
  vgui_debug_tableau(vgui_tableau_sptr const& child);

  //: Returns the type of this tableau ('vgui_debug_tableau').
  std::string type_name() const { return "vgui_debug_tableau"; }

 protected:
  //: Destructor - called by vgui_debug_tableau_sptr.
  ~vgui_debug_tableau();

  //: Handle events by printing them and then forwarding to the child tableau.
  bool handle(const vgui_event& e);
  int verbosity;
};

//: Creates a smart-pointer to a vgui_debug_tableau.
struct vgui_debug_tableau_new : public vgui_debug_tableau_sptr
{
  typedef vgui_debug_tableau_sptr base;
  vgui_debug_tableau_new(vgui_tableau_sptr const& child) : base(new vgui_debug_tableau(child)) { }
};

#endif // vgui_debug_tableau_h_
