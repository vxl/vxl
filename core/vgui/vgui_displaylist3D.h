#ifndef vgui_displaylist3D_h_
#define vgui_displaylist3D_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_displaylist3D - Undocumented class FIXME
// .LIBRARY vgui
// .HEADER vxl Package
// .INCLUDE vgui/vgui_displaylist3D.h
// .FILE vgui_displaylist3D.cxx
//
// .SECTION Description:
//
// vgui_displaylist3D is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author:
//              Philip C. Pritchett, 14 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include "vgui_displaybase.h"

#include <vgui/vgui_drag_mixin.h>

class vgui_soview3D;

class vgui_displaylist3D : public vgui_displaybase, public vgui_drag_mixin { 
public:
  vgui_displaylist3D();

  vcl_string type_name() const { return "vgui_displaylist3D"; }

  bool handle(const vgui_event& e);
  bool mouse_down(int x, int y, vgui_button button, vgui_modifier modifier);

protected:
 ~vgui_displaylist3D();

private:
  void get_hits(float x, float y, vcl_vector<unsigned>& my_hits);
};

#endif // vgui_displaylist3D_h_
