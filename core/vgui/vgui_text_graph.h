// This is oxl/vgui/vgui_text_graph.h
#ifndef vgui_text_graph_h_
#define vgui_text_graph_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vcl_iosfwd.h>
#include <vgui/vgui_tableau_sptr.h>

void vgui_text_graph(vcl_ostream &);
void vgui_text_graph(vcl_ostream &, vgui_tableau_sptr const &);

#endif // vgui_text_graph_h_
