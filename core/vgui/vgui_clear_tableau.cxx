// This is oxl/vgui/vgui_clear_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   11 Nov 99
// \brief  See vgui_clear_tableau.h for a description of this class.

#include "vgui_clear_tableau.h"
#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_dialog.h>
#include <vgui/internals/vgui_accelerate.h>

vgui_clear_tableau::vgui_clear_tableau()
{
  // These are the initial glClear() colour
  // values given in the OpenGL man pages.
  colour[0] = 0;
  colour[1] = 0;
  colour[2] = 0;
  colour[3] = 0;

  clearing_ = true;
  mask = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
  depth = 1;
  stencil = 0;
}

vgui_clear_tableau::~vgui_clear_tableau()
{
}

vcl_string vgui_clear_tableau::type_name() const { return "vgui_clear_tableau"; }


bool vgui_clear_tableau::handle(const vgui_event& event)
{
  if (event.type==vgui_DRAW && clearing_)
  {
    if (mask & GL_COLOR_BUFFER_BIT)
      glClearColor(colour[0], colour[1], colour[2], colour[3]);

    if (mask & GL_DEPTH_BUFFER_BIT)
      glClearDepth(depth);

    if (mask & GL_ACCUM_BUFFER_BIT)
      glClearAccum(accum[0], accum[1], accum[2], accum[3]);

    if (mask & GL_STENCIL_BUFFER_BIT)
      glClearStencil(stencil);

#if 0
      GLint vp[4]; // current viewport
      glGetIntegerv(GL_VIEWPORT,vp);

      GLint sc[4]; // old scissors
      glGetIntegerv(GL_SCISSOR_BOX,sc);
      bool sc_enabled = glIsEnabled(GL_SCISSOR_TEST);

      // turn on scissor test and set the scissor box to the viewport :
      glEnable(GL_SCISSOR_TEST);
      glScissor(vp[0], vp[1], vp[2], vp[3]);
#endif

    if (mask)
      vgui_accelerate::instance()->vgui_glClear(mask);

#if 0
    // turn off the scissor test, if it wasn't already on, and
    // restore old scissor settings :
    if (!sc_enabled)
      glDisable(GL_SCISSOR_TEST);
      glScissor(sc[0], sc[1], sc[2], sc[3]);
#endif

    return true;
  }

  return false;
}

void vgui_clear_tableau::set_colour(float r, float g, float b, float a)
{
  colour[0] = r;
  colour[1] = g;
  colour[2] = b;
  colour[3] = a;
}

void vgui_clear_tableau::set_mask(GLbitfield m)
{
  mask = m;
}


void vgui_clear_tableau::add_popup(vgui_menu &menu)
{
  menu.add("Configure",
           new vgui_command_simple<vgui_clear_tableau>(this, &vgui_clear_tableau::config_dialog));

  vcl_string clear_label("Toggle clearing ");
  if (clearing_) clear_label+="[on]";
  else clear_label+="[off]";

  menu.add(clear_label.c_str(),
           new vgui_command_simple<vgui_clear_tableau>(this, &vgui_clear_tableau::toggle_clearing));
}


void vgui_clear_tableau::toggle_clearing()
{
  clearing_ = !clearing_;
  post_redraw();
}

void vgui_clear_tableau::config_dialog()
{
  bool colour_val = (mask & GL_COLOR_BUFFER_BIT) != 0;
  bool depth_val = (mask & GL_DEPTH_BUFFER_BIT) != 0;
  bool accum_val = (mask & GL_ACCUM_BUFFER_BIT) != 0;
  bool stencil_val = (mask & GL_STENCIL_BUFFER_BIT) != 0;

  vgui_dialog mydialog("Clear Config");
  mydialog.checkbox("Colour",  colour_val);
  mydialog.checkbox("Depth", depth_val);
  mydialog.checkbox("Accum", accum_val);
  mydialog.checkbox("Stencil", stencil_val);

  if (mydialog.ask())
  {
    mask = 0;
    if (colour_val) mask |= GL_COLOR_BUFFER_BIT;
    if (depth_val) mask |= GL_DEPTH_BUFFER_BIT;
    if (accum_val) mask |= GL_ACCUM_BUFFER_BIT;
    if (stencil_val) mask |= GL_STENCIL_BUFFER_BIT;
  }
}
