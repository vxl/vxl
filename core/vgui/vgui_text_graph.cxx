// This is core/vgui/vgui_text_graph.cxx
//:
// \file
// \author fsm
// \brief  See vgui_text_graph.h for a description of this file.

#include <iostream>
#include "vgui_text_graph.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_tableau.h>

//-----------------------------------------------------------------------------
//: Sends a text description of the tableau hierarchy beneath the given tableau.
static
void vgui_text_graph(std::ostream &s, std::string const &prefix,
                     vgui_tableau_sptr const &t)
{
  s << prefix << t/*->type_name()*/ << std::endl;

  std::vector<vgui_tableau_sptr> tmp;
  t->get_children(&tmp);

  for (unsigned int i=0; i<tmp.size(); ++i) {
    if (i+1 < tmp.size())
      vgui_text_graph(s, prefix + "| ", tmp[i]);
    else
      vgui_text_graph(s, prefix + "  ", tmp[i]);
  }
}

//-----------------------------------------------------------------------------
//: Sends a text description of the tableau hierarchy beneath the given tableau.
void vgui_text_graph(std::ostream &s, vgui_tableau_sptr const &t)
{
  vgui_text_graph(s, "", t);
}

//-----------------------------------------------------------------------------
//: Sends a text description of the whole tableau hierarchy to the given stream.
void vgui_text_graph(std::ostream &s)
{
  s << "All tableaux in the world:\n";
  std::vector<vgui_tableau_sptr> all;
  vgui_tableau::get_all(&all);

  // find those tableaux with no parents.
  std::vector<vgui_tableau_sptr> top;
  for (unsigned int i=0; i<all.size(); ++i)
  {
    std::vector<vgui_tableau_sptr> tmp;
    all[i]->get_parents(&tmp);
    if (tmp.empty())
      top.push_back(all[i]);
  }

  for (unsigned int i=0; i<top.size(); ++i)
  {
    if (i > 0)
      s << std::endl;
    vgui_text_graph(s, top[i]);
  }
  s << std::endl;
}
