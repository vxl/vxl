#ifndef vgui_drag_tableau_h_
#define vgui_drag_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    vgui_drag_tableau - Provide a functional interface to handle
// .LIBRARY vgui
// .HEADER  vxl Package
// .INCLUDE vgui/vgui_drag_tableau.h
// .FILE    vgui_drag_tableau.cxx
//
// .SECTION Description
//    A vgui_drag_tableau is a bare tableau which uses the vgui_drag_mixin.
//    It is useful for making tableaux which have simple interaction functionality
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 11 Jan 00
//
// .SECTION Modifications
//     000111 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_drag_mixin.h>

class vgui_drag_tableau : public vgui_tableau, public vgui_drag_mixin {
public:
  vgui_drag_tableau();

protected:
  ~vgui_drag_tableau();
  bool handle(const vgui_event& e);
};

#endif // vgui_drag_tableau_h_
