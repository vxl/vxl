// This is core/vgui/internals/vgui_adaptor_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_adaptor_tableau.h for a description of this file.

#include "vgui_adaptor_tableau.h"
#include <vcl_iostream.h>
#include <vgui/vgui_adaptor.h>

//:
// The sole raison d'etre of the adaptor_tableau is to propagate
// tableau posts onto an adaptor.  It is constructed only
// by the adaptor, under which it is directly placed.
vgui_adaptor_tableau::vgui_adaptor_tableau(vgui_adaptor *a)
  : adaptor(a)
  , slot(this)
{
  //vcl_cerr << "foo_slot = " << slot << vcl_endl;
}

vgui_adaptor_tableau::~vgui_adaptor_tableau()
{
  vcl_cerr << "~vgui_adaptor_tableau(this = " << (void*)this << ")\n";
}

//: Take a message (which is invariably being sent from a child tableau), and call the adaptor's post_message(...) method.
void vgui_adaptor_tableau::post_message(char const *msg, void const *data)
{
  if (adaptor)
    adaptor->post_message(msg, data);
}

//: Take a redraw message (which is invariably being sent from a child tableau), and call the adaptor's post_redraw() method.
void vgui_adaptor_tableau::post_redraw()
{
  if (adaptor)
    adaptor->post_redraw();
}

//: Take an overlay_redraw message (which is invariably being sent from a child tableau) & call the adaptor's post_redraw() method.
void vgui_adaptor_tableau::post_overlay_redraw()
{
  if (adaptor)
    adaptor->post_overlay_redraw();
}

void vgui_adaptor_tableau::post_idle_request()
{
  if (adaptor)
    adaptor->post_idle_request();
}
