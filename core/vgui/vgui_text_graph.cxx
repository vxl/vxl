// This is oxl/vgui/vgui_text_graph.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_text_graph.h for a description of this file.

#include "vgui_text_graph.h"

#include <vcl_iostream.h>
#include <vgui/vgui_tableau.h>

//-----------------------------------------------------------------------------
//: Sends a text description of the tableau heirachy beneath the given tableau.
static
void vgui_text_graph(vcl_ostream &s, vcl_string const &prefix,
  vgui_tableau_sptr const &t)
{
  s << prefix << t/*->type_name()*/ << vcl_endl;

  vcl_vector<vgui_tableau_sptr> tmp;
  t->get_children(&tmp);

  for (unsigned int i=0; i<tmp.size(); ++i) {
    if (i+1 < tmp.size())
      vgui_text_graph(s, prefix + "| ", tmp[i]);
    else
      vgui_text_graph(s, prefix + "  ", tmp[i]);
  }
}

//-----------------------------------------------------------------------------
//: Sends a text description of the tableau heirachy beneath the given tableau.
void vgui_text_graph(vcl_ostream &s, vgui_tableau_sptr const &t)
{
  vgui_text_graph(s, "", t);
}

//-----------------------------------------------------------------------------
//: Sends a text description of the whole tableau hierachy to the given stream.
void vgui_text_graph(vcl_ostream &s)
{
  s << "All tableaux in the world:\n";
  vcl_vector<vgui_tableau_sptr> all;
  vgui_tableau::get_all(&all);

  // find those tableaux with no parents.
  vcl_vector<vgui_tableau_sptr> top;
  for (unsigned int i=0; i<all.size(); ++i) 
  {
    vcl_vector<vgui_tableau_sptr> tmp;
    all[i]->get_parents(&tmp);
    if (tmp.empty())
      top.push_back(all[i]);
  }

  for (unsigned int i=0; i<top.size(); ++i) 
  {
    if (i > 0)
      s << vcl_endl;
    vgui_text_graph(s, top[i]);
  }
  s << vcl_endl;
}
