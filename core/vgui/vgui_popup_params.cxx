//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// .NAME vgui_popup_params
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
