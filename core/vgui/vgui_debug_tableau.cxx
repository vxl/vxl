// This is oxl/vgui/vgui_debug_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   08 Oct 99
//-----------------------------------------------------------------------------

#include "vgui_debug_tableau.h"

#include <vcl_iostream.h>
#include <vcl_functional.h>

#include <vgui/vgui_event.h>

// Default ctor
vgui_debug_tableau::vgui_debug_tableau(vgui_tableau_sptr const& child):
  vgui_wrapper_tableau(child)
{
  verbosity = 1;
}

vgui_debug_tableau::~vgui_debug_tableau()
{
}

static void print_tableau(vcl_ostream& s, vcl_string indent, vgui_tableau_sptr t)
{
  s << indent << t->pretty_name() << vcl_endl;

  // Print any children
  vcl_vector<vgui_tableau_sptr> children;
  t->get_children(&children);

  if (children.size() > 1)
    indent += "     ";
  else
    indent += " ";

  for (unsigned int i = 0; i < children.size(); ++i)
    print_tableau(s, indent, children[i]);
}

static void print_tableau(vgui_tableau_sptr t)
{
  print_tableau(vcl_cerr, __FILE__ ": ", t);
}

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
    vcl_cerr << __FILE__ ": " << (handled ? "TOOK" : "left") << ": " << e << vcl_endl;

  if (!(e == e_in))
    vcl_cerr << __FILE__ ": The event changed !!!\n";

  // Handle our events
  // In general, we'd like a debug tableau to have no effect on its child,
  // so we pass the events through
  if (e.type == vgui_KEY_PRESS)
    switch (e.key) {
    case 'v':
      if (++verbosity > 2) verbosity = 0;
      vcl_cerr << __FILE__ ": verbosity = " << verbosity << vcl_endl;
      break;
    case 'p':
      print_tableau(this);
      break;
    default:
      break; // quell warning
    }

  return handled;
}
