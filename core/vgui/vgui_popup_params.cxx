// This is oxl/vgui/vgui_popup_params.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 21 Jan 00
//
//-----------------------------------------------------------------------------

#include "vgui_popup_params.h"


vgui_popup_params::vgui_popup_params()
  : x(-1) , y(-1)
  , recurse(true)
  , nested(false) 
  , defaults(false)
{}
