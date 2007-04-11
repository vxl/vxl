// This is oxl/ogui/impl/glX/vgui_glX_adaptor.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   11 Sep 99
//-----------------------------------------------------------------------------

#include "vgui_glX_adaptor.h"

#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>

#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_event.h>

#include "vgui_glX.h"
#include "vgui_glX_window.h"

static bool debug = false;


vgui_glX_adaptor::vgui_glX_adaptor() :
  use_popups(true)
{
}


vgui_glX_adaptor::~vgui_glX_adaptor()
{
}


void vgui_glX_adaptor::make_current()
{
  Display *display = vgui_glX::instance()->display;
  Window win = window->window;
  GLXContext context = window->context;

  glXMakeCurrent(display, win, context);
}

void vgui_glX_adaptor::swap_buffers()
{
  Display *display = vgui_glX::instance()->display;
  Window win = window->window;

  glXSwapBuffers(display,win);
}


void vgui_glX_adaptor::draw()
{
  if (debug) vcl_cerr << "vgui_glX_adaptor::draw\n";

  make_current();

  vgui_matrix_state::clear_gl_matrices();

  if (get_tableau()) {
    vgui_event e;
    e.type = vgui_DRAW;
    dispatch_to_tableau(e);
  }

  swap_buffers();
}
