// This is core/vgui/vgui_observer.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   11 Sep 99
// \brief  See vgui_observer.h for a description of this file.

#include "vgui_observer.h"
#include "vgui/vgui_message.h"

void
vgui_observer::update()
{
  // do nothing
}

void
vgui_observer::update(const vgui_message &)
{
  this->update(); // default implementation.
}

void
vgui_observer::update(const vgui_observable *)
{
  this->update(); // default implementation.
}
