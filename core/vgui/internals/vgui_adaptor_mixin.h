// This is oxl/vgui/internals/vgui_adaptor_mixin.h
#ifndef vgui_adaptor_mixin_h_
#define vgui_adaptor_mixin_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \brief  Holds the dimensions of the adaptor and the event causing popup menus.

#include <vgui/vgui_button.h>
#include <vgui/vgui_modifier.h>

//: Holds the dimensions of the adaptor and the event causing popup menus.
class vgui_adaptor_mixin
{
 public:
  vgui_adaptor_mixin();
  ~vgui_adaptor_mixin();

  //: Cached width of adaptor render area.
  unsigned width;
  //: Cached height of adaptor render area.
  unsigned height;

  // These specify the event that causes the popup menu to appear

  //: Mouse button (used with popup_modifier) which causes the popup menu to appear.
  vgui_button popup_button;
  //: Modifier (used with popup_button) which causes the popup menu to appear.
  vgui_modifier popup_modifier;
};

#endif // vgui_adaptor_mixin_h_
