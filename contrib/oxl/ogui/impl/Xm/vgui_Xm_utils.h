// This is oxl/ogui/impl/Xm/vgui_Xm_utils.h
#ifndef vgui_Xm_utils_h_
#define vgui_Xm_utils_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author
//              Philip C. Pritchett, 02 Nov 99
//              Robotics Research Group, University of Oxford
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
