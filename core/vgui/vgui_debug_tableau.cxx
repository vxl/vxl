// This is core/vgui/vgui_debug_tableau.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Oct 1999
// \brief  See vgui_debug_tableau.cxx for a description of this file.

#include <iostream>
#include "vgui_debug_tableau.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_event.h>

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_debug_tableau_new.
//  Takes the one and only child of this tableau.
vgui_debug_tableau::vgui_debug_tableau(vgui_tableau_sptr const& child_tableau):
  vgui_wrapper_tableau(child_tableau)
{
  verbosity = 1;
}

//-----------------------------------------------------------------------------
//: Destructor - called by vgui_debug_tableau_sptr.
vgui_debug_tableau::~vgui_debug_tableau()
{
}

//-----------------------------------------------------------------------------
static void print_tableau(std::ostream& s, std::string indent, vgui_tableau_sptr t)
{
  s << indent << t->pretty_name() << std::endl;

  // Print any children
  std::vector<vgui_tableau_sptr> children;
  t->get_children(&children);

  if (children.size() > 1)
    indent += "     ";
  else
    indent += " ";

  for (unsigned int i = 0; i < children.size(); ++i)
    print_tableau(s, indent, children[i]);
}

//-----------------------------------------------------------------------------
static void print_tableau(vgui_tableau_sptr t)
{
  print_tableau(std::cerr, __FILE__ ": ", t);
}

//-----------------------------------------------------------------------------
//: Handle events by printing them and then forwarding to the child tableau.
bool vgui_debug_tableau::handle(const vgui_event& e)
{
  vgui_event e_in = e;

  // First run the event through the child :
  bool handled = child && child->handle(e);

  // Print the event
  bool print = (verbosity > 0);
  if (e.type == vgui_MOTION)
    print = (verbosity > 1);
  if (print)
    std::cerr << __FILE__ ": " << (handled ? "TOOK" : "left") << ": " << e << std::endl;

  if (!(e == e_in))
    std::cerr << __FILE__ ": The event changed !!!\n";

  // Handle our events
  // In general, we'd like a debug tableau to have no effect on its child,
  // so we pass the events through
  if (e.type == vgui_KEY_PRESS)
    switch (e.key) {
    case 'v':
      if (++verbosity > 2) verbosity = 0;
      std::cerr << __FILE__ ": verbosity = " << verbosity << std::endl;
      break;
    case 'p':
      print_tableau(this);
      break;
    default:
      break; // quell warning
    }

  return handled;
}
