// This is oxl/vgui/internals/vgui_string_field.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 24 Oct 99
//
//-----------------------------------------------------------------------------

#include "vgui_string_field.h"

vgui_string_field::vgui_string_field(const char* text, vcl_string& variable_to_modify)
  : vgui_dialog_field(text)
  , var(variable_to_modify)
{
}

vgui_string_field::~vgui_string_field()
{
}

vcl_string vgui_string_field::current_value() const
{
  return var;
}

bool vgui_string_field::update_value(const vcl_string &s)
{
  var = s;
  return true;
}
