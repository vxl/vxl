// This is core/vgui/internals/vgui_adaptor_mixin.h
#ifndef vgui_adaptor_mixin_h_
#define vgui_adaptor_mixin_h_
//:
// \file
// \author fsm
// \brief  Holds the dimensions of the adaptor and the event causing popup menus.
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vgui/vgui_button.h>
#include <vgui/vgui_modifier.h>

//: Holds the dimensions of the adaptor and the event causing popup menus.
class vgui_adaptor_mixin
{
 public:
  vgui_adaptor_mixin() : width(0), height(0),
                         popup_button(vgui_RIGHT),
                         popup_modifier(vgui_MODIFIER_NULL) {}
  ~vgui_adaptor_mixin() {}

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
