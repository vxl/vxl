#ifndef vgui_text_graph_h_
#define vgui_text_graph_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/
// .NAME vgui_text_graph
// .INCLUDE vgui/vgui_text_graph.h
// .FILE vgui_text_graph.cxx

#include <vcl_iosfwd.h>
#include <vgui/vgui_tableau_sptr.h>

void vgui_text_graph(vcl_ostream &);
void vgui_text_graph(vcl_ostream &, vgui_tableau_sptr const &);

#endif // vgui_text_graph_h_
