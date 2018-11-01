// This is core/vgui/internals/vgui_overlay_helper.cxx
//:
// \file
// \author fsm

#include <iostream>
#include "vgui_overlay_helper.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_event.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/internals/vgui_accelerate.h>

#define trace if (true) { } else std::cerr

vgui_overlay_helper::vgui_overlay_helper(vgui_adaptor *a)
  : adaptor(a)
  //, aux_buffer_is_back_buffer(true)
  , last_draw_was_overlay(false)
  , overlay_redraw_posted(false)
{
  trace << "vgui_overlay_helper created\n";
  assert(adaptor);

  // check that this GL state is double buffered :
  adaptor->make_current();
  {
    GLboolean isdouble = 0;
    glGetBooleanv(GL_DOUBLEBUFFER, &isdouble);
    if (!isdouble) {
      vgui_macro_warning << "current GL state is not double buffered\n";
      return;
    }
  }
}

vgui_overlay_helper::~vgui_overlay_helper() {
  adaptor = nullptr;
}

//--------------------------------------------------------------------------------

// this could be inlined
bool vgui_overlay_helper::dispatch(vgui_event const &e) {
  if (e.type == vgui_DRAW)
    return dispatch_draw(e);

  else if (e.type == vgui_DRAW_OVERLAY)
    return dispatch_overlay_draw(e);

  else
    return dispatch_other(e);
}

bool vgui_overlay_helper::dispatch_draw(vgui_event const &e) {
  //vgui_macro_warning << "emulation normal redisplay" << endl;
  assert(e.type == vgui_DRAW);

  // First perform the normal draw :
  last_draw_was_overlay = false;
  bool f = adaptor->dispatch_to_tableau(e);
  adaptor->swap_buffers(); // should this be moved further down?

  // It is up to tableaux to post overlay redraws when receiving
  // vgui_DRAW events, so if any overlays need to be drawn, we
  // should know at this point.
  if (overlay_redraw_posted) {
    vgui_event oe(vgui_DRAW_OVERLAY);
    f = dispatch_overlay_draw(oe);
  }

  return f;
}

bool vgui_overlay_helper::dispatch_other(vgui_event const &e) {
  //vgui_macro_warning << "emulation other dispatch" << endl;
  assert(e.type != vgui_DRAW && e.type != vgui_DRAW_OVERLAY);

  // dispatch
  bool f = adaptor->dispatch_to_tableau(e);

  // cf dispatch_draw
  if (overlay_redraw_posted) {
    vgui_event oe(vgui_DRAW_OVERLAY);
    f = dispatch_overlay_draw(oe);
  }

  return f;
}

bool vgui_overlay_helper::dispatch_overlay_draw(vgui_event const &e) {
  //vgui_macro_warning << "emulation overlay redisplay" << endl;
  assert(e.type == vgui_DRAW_OVERLAY);

  // If the last draw was a normal draw we need to snapshot the
  // frame buffer before drawing the overlays.
  if (!last_draw_was_overlay) {
    trace << "snapshot\n";
    if (vgui_accelerate::instance()->vgui_copy_back_to_aux())
      aux_buffer_is_back_buffer = false;
    else {
      vgui_utils::copy_front_to_back();
      aux_buffer_is_back_buffer = true;
    }
  }
  // Else, the last draw was an overlay draw, so we need to repair
  // the damage before drawing the new overlays.
  else {
    trace << "revert\n";
    if (!aux_buffer_is_back_buffer)
      vgui_accelerate::instance()->vgui_copy_aux_to_back();
    else
      vgui_utils::copy_back_to_front();
  }


  // enter "overlay mode"
  if (!aux_buffer_is_back_buffer)
    glDrawBuffer(GL_BACK);
  else
    glDrawBuffer(GL_FRONT);


  // get tableau to draw its overlays.
  bool f = adaptor->dispatch_to_tableau(e);


  // With an auxiliary buffer (Mesa), we now need to blit the overlays
  // rendered in the back buffer onto the screen.
  if (!aux_buffer_is_back_buffer)
    adaptor->swap_buffers();


  // update state
  last_draw_was_overlay = true;  // remember this for later.
  overlay_redraw_posted = false; // overlay draw no longer pending.


  // leave "overlay mode"
  if (aux_buffer_is_back_buffer)
    glDrawBuffer(GL_BACK);

  return f;
}

//--------------------------------------------------------------------------------
