// This is oxl/vgui/vgui_popup_params.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   21 Jan 00
// \brief  See vgui_popup_params.h for a description of this file.

#include "vgui_popup_params.h"


vgui_popup_params::vgui_popup_params()
  : x(-1) , y(-1)
  , recurse(true)
  , nested(false) 
  , defaults(false)
{}
