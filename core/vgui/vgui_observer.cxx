// This is oxl/vgui/vgui_observer.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 11 Sep 99
//
//-----------------------------------------------------------------------------

#include "vgui_observer.h"
#include "vgui_message.h"

vgui_observer::vgui_observer()
{
}

vgui_observer::~vgui_observer()
{
}

void vgui_observer::update()
{
  // do nothing
}

void vgui_observer::update(vgui_message const &)
{
  this->update(); // default implementation.
}

void vgui_observer::update(vgui_observable const *)
{
  this->update(); // default implementation.
}
