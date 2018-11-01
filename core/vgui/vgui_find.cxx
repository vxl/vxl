// This is core/vgui/vgui_find.cxx
//:
// \file
// \author fsm
// \brief  See vgui_find.h for a description of this file.

#include "vgui_find.h"
#include <vgui/vgui_tableau.h>

// Does a depth-first search for the first tableau whose type_name()
// method returns the given std::string. Returns 0 if no tableau is found.
vgui_tableau_sptr vgui_find_by_type_name(vgui_tableau_sptr const& start, std::string const &tn, bool direction_down)
{
  if (!start)
    return nullptr;

  if (start->type_name() == tn)
    return start;

  std::vector<vgui_tableau_sptr> tt;
  if (direction_down)
    start->get_children(&tt); // get all children.
  else
    start->get_parents (&tt); // get all parents.

  for (unsigned int i=0; i<tt.size(); ++i) {
    vgui_tableau_sptr t = vgui_find_by_type_name(tt[i], tn, direction_down);
    if (t)
      return t; // found one.
  }
  return nullptr; // not found.
}


vgui_tableau_sptr
vgui_find_by_name(vgui_tableau_sptr const& start, std::string const &name, bool direction_down)
{
  if (!start)
    return nullptr;

  if (start->name() == name)
    return start;

  std::vector<vgui_tableau_sptr> tt;
  if (direction_down)
    start->get_children(&tt); // get all children.
  else
    start->get_parents (&tt); // get all parents.

  for (unsigned int i=0; i<tt.size(); ++i) {
    vgui_tableau_sptr t = vgui_find_by_name(tt[i], name, direction_down);
    if (t)
      return t; // found one.
  }
  return nullptr; // not found.
}
