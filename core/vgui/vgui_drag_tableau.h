// This is oxl/vgui/vgui_drag_tableau.h
#ifndef vgui_drag_tableau_h_
#define vgui_drag_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   11 Jan 00
// \brief  A bare tableau which uses the vgui_drag_mixin.
//         Contains classes: vgui_drag_tableau.
//
// \verbatim
//  Modifications
//    000111 AWF Initial version.
//    26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_drag_mixin.h>

//: A bare tableau which uses the vgui_drag_mixin
//  It is useful for making tableaux which have simple interaction 
//  functionality.  It is expected that this tableau would be used 
//  as a base class for your own derived tableau, not that this
//  tableau would be used directly.
class vgui_drag_tableau : public vgui_tableau, public vgui_drag_mixin
{
 public:
  vgui_drag_tableau();

 protected:
  ~vgui_drag_tableau();
  bool handle(const vgui_event& e);
};

#endif // vgui_drag_tableau_h_
