// This is oxl/vgui/vgui_slab.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  See vgui_slab.h for a description of this file.

#include "vgui_slab.h"

#include <vcl_vector.h>

vgui_slab::vgui_slab()
{
  // register ?
}

vgui_slab::~vgui_slab()
{
  // unregister ?
}

int  vgui_slab::post_timer(float t)
{
  static int counter = 0;
  post_timer(t, counter);
  return counter++;
}

void vgui_slab::post_timer(float, int) { }

void vgui_slab::post_redraw()
{
  vgui_event e(vgui_DRAW);
  queue_push(e);
}

void vgui_slab::post_overlay_redraw()
{
  vgui_event e(vgui_OVERLAY_DRAW);
  queue_push(e);
}

#if 0
//: block until one of the given slabs has an event ready.
vgui_slab *vgui_blocking_wait(vcl_vector<vgui_slab*> const &which /*, double timeout*/)
{
  while (true /*timeout not happened yet*/) {
    for (int i=0; i<which.size(); ++i)
      if (! which[i]->queue_empty())
        return which[i];

    // no slabs have events. sleep for a millisecond, to avoid
    // maxing out the cpu.
    // vcl_usleep(1);
  }
  return 0;
}
#endif
