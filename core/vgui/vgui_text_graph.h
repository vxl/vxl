// This is core/vgui/vgui_text_graph.h
#ifndef vgui_text_graph_h_
#define vgui_text_graph_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#  pragma interface
#endif
// :
// \file
// \author fsm
// \brief  Text descriptions of the tableau hierarchy.

#include <vcl_iosfwd.h>
#include <vgui/vgui_tableau_sptr.h>

// : Sends a text description of the whole tableau hierarchy to the given stream.
void vgui_text_graph(vcl_ostream &);

// : Sends a text description of tableau hierarchy beneath the given tableau.
void vgui_text_graph(vcl_ostream &, vgui_tableau_sptr const &);

#endif // vgui_text_graph_h_
