// This is oxl/vgui/vgui_adaptor_mixin.cxx
//
// \author fsm@robots.ox.ac.uk

#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_adaptor_mixin.h"

vgui_adaptor_mixin::vgui_adaptor_mixin()
  : width(0)
  , height(0)
  //
  , popup_button(vgui_RIGHT)
  , popup_modifier(vgui_MODIFIER_NULL)
{
}

vgui_adaptor_mixin::~vgui_adaptor_mixin()
{
}
