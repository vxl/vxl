#ifndef vgui_displaylist2D_h_
#define vgui_displaylist2D_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_displaylist2D - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_displaylist2D.h
// .FILE vgui_displaylist2D.cxx
//
// .SECTION Description
//
// vgui_displaylist2D is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 14 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//    1-SEP-2000 -- Marko Bacic,Oxford RRG -- Added displaying of object
//                  information
//-----------------------------------------------------------------------------

#include "vgui_displaybase.h"

#include <vgui/vgui_drag_mixin.h>
#include <vgui/vgui_displaylist2D_ref.h>

class vgui_displaylist2D : public vgui_displaybase { 
public:
  vgui_displaylist2D();

  virtual bool handle(const vgui_event& e);
  bool motion(int x, int y);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

protected:
 ~vgui_displaylist2D();

private:
  void get_hits(float x, float y, vcl_vector<unsigned>& my_hits);
  unsigned find_closest(float x, float y, vcl_vector<unsigned>& hits);

  bool posted_redraw_;
};

struct vgui_displaylist2D_new : public vgui_displaylist2D_ref {
  vgui_displaylist2D_new() : 
    vgui_displaylist2D_ref(new vgui_displaylist2D) { } 
};

#endif // vgui_displaylist2D_h_
