// This is ./oxl/vgui/vgui_drag_tableau.h

//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   11 Jan 00
// \brief  A bare tableau which uses the vgui_drag_mixin.
//
// \verbatim
//  Modifications
//    000111 AWF Initial version.
//    26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#ifndef vgui_drag_tableau_h_
#define vgui_drag_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_drag_mixin.h>

//: A bare tableau which uses the vgui_drag_mixin
//  It is useful for making tableaux which have simple interaction functionality
class vgui_drag_tableau : public vgui_tableau, public vgui_drag_mixin {
public:
  vgui_drag_tableau();

protected:
  ~vgui_drag_tableau();
  bool handle(const vgui_event& e);
};

#endif // vgui_drag_tableau_h_
