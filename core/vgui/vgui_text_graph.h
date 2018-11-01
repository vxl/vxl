// This is core/vgui/vgui_text_graph.h
#ifndef vgui_text_graph_h_
#define vgui_text_graph_h_
//:
// \file
// \author fsm
// \brief  Text descriptions of the tableau hierarchy.

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_tableau_sptr.h>

//: Sends a text description of the whole tableau hierarchy to the given stream.
void vgui_text_graph(std::ostream &);

//: Sends a text description of tableau hierarchy beneath the given tableau.
void vgui_text_graph(std::ostream &, vgui_tableau_sptr const &);

#endif // vgui_text_graph_h_
