//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vgui_drag_tableau
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 11 Jan 00
//
//-----------------------------------------------------------------------------

#include "vgui_drag_tableau.h"

// Default ctor
vgui_drag_tableau::vgui_drag_tableau()
{
}

// Destructor
vgui_drag_tableau::~vgui_drag_tableau()
{
}

bool vgui_drag_tableau::handle(const vgui_event& e)
{
  if (vgui_drag_mixin::handle(e))
    return true;

  return vgui_tableau::handle(e);
}
