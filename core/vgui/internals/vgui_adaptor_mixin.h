#ifndef vgui_adaptor_mixin_h_
#define vgui_adaptor_mixin_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vgui/vgui_event.h>

class vgui_adaptor_mixin {
public:
  vgui_adaptor_mixin();
  ~vgui_adaptor_mixin();

  // Cached dimensions of adaptor render area.
  unsigned width;
  unsigned height;

  // These specify the event that causes the popup menu to appear
  vgui_button popup_button;
  vgui_modifier popup_modifier;
};

#endif // vgui_adaptor_mixin_h_
