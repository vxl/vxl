// This is oxl/vgui/internals/vgui_adaptor_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  See vgui_adaptor_tableau.h for a description of this file.

#include "vgui_adaptor_tableau.h"
#include <vcl_iostream.h>
#include <vgui/vgui_adaptor.h>

//:
// The sole raison d'etre of the adaptor_tableau is to propagate
// tableau posts onto an adaptor.

vgui_adaptor_tableau::vgui_adaptor_tableau(vgui_adaptor *a)
  : adaptor(a)
  , slot(this, 0)
{
  //vcl_cerr << "foo_slot = " << slot << vcl_endl;
}

vgui_adaptor_tableau::~vgui_adaptor_tableau() 
{
  vcl_cerr << "~vgui_adaptor_tableau(this = " << (void*)this << ")\n";
}

void vgui_adaptor_tableau::post_message(char const *msg, void const *data) 
{ 
  if (adaptor)
    adaptor->post_message(msg, data);
}

void vgui_adaptor_tableau::post_redraw() 
{ 
  if (adaptor)
    adaptor->post_redraw();
}

void vgui_adaptor_tableau::post_overlay_redraw() 
{ 
  if (adaptor)
    adaptor->post_overlay_redraw();
}
