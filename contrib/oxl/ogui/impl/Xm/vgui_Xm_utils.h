// This is oxl/vgui/impl/Xm/vgui_Xm_utils.h
#ifndef vgui_Xm_utils_h_
#define vgui_Xm_utils_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// 
// .NAME vgui_Xm_utils - Undocumented class FIXME
// .LIBRARY vgui-Xm
// .HEADER vxl Package
// .INCLUDE vgui/impl/Xm/vgui_Xm_utils.h
// .FILE vgui_Xm_utils.cxx
//
// .SECTION Description
// vgui_Xm_utils is a class that Phil hasnt documented properly. FIXME
//
// .SECTION Author
//              Philip C. Pritchett, 02 Nov 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <X11/IntrinsicP.h>
class vgui_menu;

class vgui_Xm_utils 
{
 public:

  static void add_submenu(Widget widget, const vgui_menu& menu);
  static void set_menu(Widget widget, const vgui_menu& menu);
};

#endif // vgui_Xm_utils_h_
