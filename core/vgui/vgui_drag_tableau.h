// This is core/vgui/vgui_drag_tableau.h
#ifndef vgui_drag_tableau_h_
#define vgui_drag_tableau_h_
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   11 Jan 00
// \brief  A bare tableau which uses the vgui_drag_mixin.
//
//         Contains classes: vgui_drag_tableau vgui_drag_tableau_new
//
// \verbatim
//  Modifications
//   000111 AWF Initial version.
//   26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
//   01-OCT-2002 K.Y.McGaul - Moved vgui_drag to vgui_drag_tableau.
//   02-OCT-2002 K.Y.McGaul - Added vgui_drag_tableau_new.
//   10-SEP-2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_drag_mixin.h>

//: A bare tableau which uses the vgui_drag_mixin
//
//  It is useful for making tableaux which have simple interaction
//  functionality.  It is expected that this tableau would be used
//  as a base class for your own derived tableau, not that this
//  tableau would be used directly.
class vgui_drag_tableau : public vgui_tableau, public vgui_drag_mixin
{
 public:
  //: Constructor - don't use this, use vgui_drag_tableau_new.
  vgui_drag_tableau() {}

 protected:
  //: Destructor - called by vgui_drag_tableau_sptr.
  ~vgui_drag_tableau() {}

  //: Handle all events sent to this tableau.
  bool handle(const vgui_event& e)
  {
    if (vgui_drag_mixin::handle(e)) return true;
    return vgui_tableau::handle(e);
  }
};

#include <vgui/vgui_drag_tableau_sptr.h>

//: Creates a smart-pointer to a vgui_drag_tableau.
struct vgui_drag_tableau_new : public vgui_drag_tableau_sptr
{
  //: Constructor - create a default vgui_drag_tableau.
  vgui_drag_tableau_new() : vgui_drag_tableau_sptr(new vgui_drag_tableau) {}
};

#endif // vgui_drag_tableau_h_
