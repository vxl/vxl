// This is core/vgui/vgui_observer.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   11 Sep 99
// \brief  See vgui_observer.h for a description of this file.

#include "vgui_observer.h"
#include <vgui/vgui_message.h>

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
