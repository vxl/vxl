/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_text_graph.h"

#include <vcl_iostream.h>
#include <vgui/vgui_tableau.h>

static
void vgui_text_graph(vcl_ostream &s, vcl_string const &prefix,vgui_tableau_ref const &t)
{
  s << prefix << t/*->type_name()*/ << vcl_endl;
  
  vcl_vector<vgui_tableau_ref> tmp;
  t->get_children(&tmp);
  
  for (int i=0; i<tmp.size(); ++i) {
    if (i < tmp.size()-1)
      vgui_text_graph(s, prefix + "| ", tmp[i]);
    else
      vgui_text_graph(s, prefix + "  ", tmp[i]);
  }
}

void vgui_text_graph(vcl_ostream &s, vgui_tableau_ref const &t)
{
  vgui_text_graph(s, "", t);
}

void vgui_text_graph(vcl_ostream &s)
{
  s << "All tableaux in the world:" << vcl_endl;
  vcl_vector<vgui_tableau_ref> all;
  vgui_tableau::get_all(&all);

  // find those tableaux with no parents.
  vcl_vector<vgui_tableau_ref> top;
  for (int i=0; i<all.size(); ++i) {
    vcl_vector<vgui_tableau_ref> tmp;
    all[i]->get_parents(&tmp);
    if (tmp.empty())
      top.push_back(all[i]);
  }
  
  for (int i=0; i<top.size(); ++i) {
    if (i > 0)
      s << vcl_endl;
    vgui_text_graph(s, top[i]);
  }
  s << vcl_endl;
}
