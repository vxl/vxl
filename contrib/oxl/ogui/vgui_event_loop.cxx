// This is oxl/ogui/vgui_event_loop.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_event_loop.h for a description of this file.

#include "vgui_event_loop.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <vgui/vgui_slot.h>
#include <vgui/vgui_slab.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_macro.h>

struct vgui_slab_tableau : public vgui_tableau
{
  vgui_slab *slab;
  vgui_slot slot;
  vgui_slab_tableau(vgui_slab *slab_, vgui_tableau_sptr const& t) : slab(slab_), slot(this, t) { }
  bool handle(vgui_event const &e) { return slot.handle(e); }
  void post_message(char const *, void const *) { }
  void post_redraw() { if (slab) slab->post_redraw(); }
  void post_overlay_redraw() { if (slab) slab->post_overlay_redraw(); }
};

typedef vcl_vector<vgui_tableau*> container;
static container the_container;

void vgui_event_loop_attach(vgui_slab *slab, vgui_tableau_sptr const& tt)
{
  for (unsigned int i=0; i<the_container.size(); ++i) {
    vgui_slab_tableau *st = (vgui_slab_tableau*) the_container[i];
    if (st->slab == slab) {
      st->slot.assign(tt);
      return;
    }
  }

  vgui_slab_tableau *st = new vgui_slab_tableau(slab, tt);
  st->ref();
  the_container.push_back(st);
}

void vgui_event_loop_detach(vgui_slab *slab)
{
  for (unsigned int i=0; i<the_container.size(); ++i) {
    vgui_slab_tableau *st = (vgui_slab_tableau*) the_container[i];
    if (st->slab == slab) {
      the_container.erase(the_container.begin() + i);
      st->unref();
      return;
    }
  }

  vgui_macro_warning << "no slab " << (void*)slab << vcl_endl;
}

//------------------------------------------------------------

void vgui_event_loop_service()
{
  if (the_container.size() == 0) {
    vgui_macro_warning << "no (slab, tableau) pairs to service\n";
    vgui_event_loop_finish();
    return;
  }

  for (unsigned int i=0; i<the_container.size(); ++i) {
    vgui_slab_tableau *st = (vgui_slab_tableau*) the_container[i];

    if ( st->slab->queue_empty() ) {
      // idle?
    }
    else {
      // get the next event on that slab
      vgui_event e;
      bool ok = st->slab->queue_next(&e);

      // despatch it, ...
      if (ok) {
        if (e.type == vgui_DRAW) {
          if (st->slab->overlay_active())
            st->slab->overlay_leave();
          st->slab->draw_begin();
          st->slot.handle(e);
          st->slab->draw_end();
        }
        else if (e.type == vgui_DRAW_OVERLAY) {
          if (! st->slab->overlay_active())
            st->slab->overlay_enter();
          st->slab->draw_begin();
          st->slot.handle(e);
          st->slab->draw_end();
        }
        else
          st->slot.handle(e);
      }
      // ...or discard it.
      else
        { /* ??? */ }

      // !! FIXME: the iterator might be invalid now !!
      break;
    }
  }
}

static bool vgui_event_loop_running = false;

int vgui_event_loop()
{
  vgui_event_loop_running = true;
  while (vgui_event_loop_running)
    vgui_event_loop_service();
  return 0;
}

void vgui_event_loop_finish()
{
  vgui_event_loop_running = false;
}
