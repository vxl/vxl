// This is core/vgui/vgui_clear_tableau.cxx
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
#include <vcl_sstream.h>

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_clear_tableau_new.
//  A vgui_clear_tableau does not have any children.
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

//-----------------------------------------------------------------------------
//: Handle events sent to this tableau - use draw to perform OpenGL clearing.
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

    if (mask)
      vgui_accelerate::instance()->vgui_glClear(mask);

    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
//: Set colour of clear tableau to the given red, green, blue values.
void vgui_clear_tableau::set_colour(float r, float g, float b, float a)
{
  colour[0] = r;
  colour[1] = g;
  colour[2] = b;
  colour[3] = a;
}

//-----------------------------------------------------------------------------
//: Make the given menu the default pop-up menu.
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

//-----------------------------------------------------------------------------
//: Toggle clearing on and off.
void vgui_clear_tableau::toggle_clearing()
{
  clearing_ = !clearing_;
  post_redraw();
}

//-----------------------------------------------------------------------------
//: Display a dialog box to get data (colour, etc) for the clear tableau.
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

  vcl_stringstream color_stm;
  color_stm << colour[0] << ' ' << colour[1] << ' ' << colour[2];
  vcl_string color = color_stm.str();
  mydialog.inline_color("Clear Colour",color);
  mydialog.field("Alpha", colour[3] );

  if (mydialog.ask())
  {
    mask = 0;
    if (colour_val) mask |= GL_COLOR_BUFFER_BIT;
    if (depth_val) mask |= GL_DEPTH_BUFFER_BIT;
    if (accum_val) mask |= GL_ACCUM_BUFFER_BIT;
    if (stencil_val) mask |= GL_STENCIL_BUFFER_BIT;

    color_stm << color;
    color_stm >> colour[0] >> colour[1] >> colour[2];
  }
}
