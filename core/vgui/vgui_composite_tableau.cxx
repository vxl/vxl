// This is core/vgui/vgui_composite_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   15 Sep 99
// \brief  See vgui_composite_tableau.h for a description of this file.


#include "vgui_composite_tableau.h"

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>

static vgui_event_condition default_c_enable_key_bindings(vgui_key_CTRL('c'));

//----------------------------------------------------------------------------
//: Prints info about this tableau - called when '?' is pressed.
bool vgui_composite_tableau::help()
{
  vcl_cerr << "\n+- vgui_composite_tableau -----+\n"
           << "|     keys                     |\n"
           << "| `1' to `9'  toggle child `n' |\n"
           << "+------------------------------+\n\n";
  return false;
}

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_composite_tableau_new.
//  Creates an empty composite tableau.
vgui_composite_tableau::vgui_composite_tableau()
  : c_enable_key_bindings(default_c_enable_key_bindings)
{
  enable_key_bindings = false;
}

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_composite_tableau_new.
//  Takes 2 children: the first is on top, the second below.
vgui_composite_tableau::vgui_composite_tableau(vgui_tableau_sptr const& child0,
                                               vgui_tableau_sptr const& child1)
  : c_enable_key_bindings(default_c_enable_key_bindings)
{
  add(child0);
  add(child1);
  enable_key_bindings = false;
}

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_composite_tableau_new.
// Three children, top to bottom.
vgui_composite_tableau::vgui_composite_tableau(vgui_tableau_sptr const& child0,
                                               vgui_tableau_sptr const& child1,
                                               vgui_tableau_sptr const& child2)
  : c_enable_key_bindings(default_c_enable_key_bindings)
{
  add(child0);
  add(child1);
  add(child2);
  enable_key_bindings = false;
}

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_composite_tableau_new.
// Many children, top to bottom.
vgui_composite_tableau::vgui_composite_tableau(vcl_vector<vgui_tableau_sptr> const& the_children)
  : c_enable_key_bindings(default_c_enable_key_bindings)
{
  for (unsigned int i = 0; i < the_children.size(); ++i)
    add(the_children[i]);
  enable_key_bindings = false;
}

//----------------------------------------------------------------------------
//: Returns a nice version of the name, including info on the children.
vcl_string vgui_composite_tableau::pretty_name() const
{
  vcl_stringstream s; s << type_name() << "[#kids=" << children.size() << ']';
  return s.str();
}

//----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
//  All events (except key-presses '0'-'9' and draw events) are passed
//  to each child until the event is handled.
//  Key presses '0'-'9' toggle the activeness of the children and
//  draw events are sent to all children.
//  Key-press '?' prints info on this file, before being sent to the children.
bool vgui_composite_tableau::handle(const vgui_event& event)
{
  // Save current matrix state. Each active child will be
  // invoked with this matrix state.
  vgui_matrix_state PM;
  unsigned int n = children.size();

  // "DRAW" events. Return true unless some child returns false.
  // What is the logic behind this? I'm no longer sure....
  // Worse, draws should be upside down...
  if (event.type==vgui_DRAW || event.type==vgui_DRAW_OVERLAY) {
    bool retv = true;

    for (unsigned ia = 0; ia<n; ++ia) {
      if (active[ia] && children[ia]) {
        PM.restore();
        if ( !children[ia]->handle(event) )
          retv=false;
      }
    }

    return retv;
  }

  // "normal" events :
  // Alt-C enables / disables key bindings
  if (c_enable_key_bindings(event)) {
    vcl_cerr << "Toggle keybindings\n";
    vgui::out << "Toggle keybindings";
    enable_key_bindings = !enable_key_bindings;
  }

  // First check if the composite itself wants it :
  if (event.type == vgui_KEY_PRESS && enable_key_bindings) {
    int key = event.key;
    if (key == '?')
      help();

    if (key >= '1' && key <= '9') {
      toggle(key-'1');
      post_redraw();
      return true;
    }
  }

  // If not, pass it on till some child handles it :
  for (int ia = (int)n-1; ia>=0; --ia) {
    if (active[ia] && children[ia]) {
      PM.restore();
      if ( children[ia]->handle(event) )
        return true;
    }
  }

  // Noone was interested....
  return false;
}

//----------------------------------------------------------------------------
//: Calls notify for the observers.
void vgui_composite_tableau::notify() const
{
  observers.notify();
}

//----------------------------------------------------------------------------
//: Returns a bounding box large enough to contain all the child bounding boxes.
bool vgui_composite_tableau::get_bounding_box(float lo[3], float hi[3]) const
{
  // it would be nice if we could return an empty bounding box.
  if (children.empty())
    return false;

  // get bbox of first child.
  if (!children[0]->get_bounding_box(lo, hi))
    return false;

  for (unsigned i=1; i<children.size(); ++i) {
    // get bbox of child i.
    float l[3], h[3];
    if (!children[i]->get_bounding_box(l, h))
      return false;

    // enlarge if necessary.
    for (unsigned j=0; j<3; ++j) {
      if (l[j] < lo[j]) lo[j] = l[j];
      if (h[j] > hi[j]) hi[j] = h[j];
    }
  }
  return true;
}

//----------------------------------------------------------------------------
//: Add to list of child tableaux.
// virtual
bool vgui_composite_tableau::add_child(vgui_tableau_sptr const& t)
{
  children.push_back( vgui_parent_child_link(this,t) );
  active.push_back(true);
  notify();
  return true;
}

//----------------------------------------------------------------------------
//: Remove given tableau from list of child tableaux.
void vgui_composite_tableau::remove(vgui_tableau_sptr const& t)
{
  if (!remove_child(t))
    vcl_cerr << __FILE__ " : no such child tableau\n";
}

//----------------------------------------------------------------------------
void vgui_composite_tableau::clear()
{
  children.clear();
  active.clear();
  notify();
}

//----------------------------------------------------------------------------
// virtual
bool vgui_composite_tableau::remove_child(vgui_tableau_sptr const &t)
{
  vcl_vector<bool>::iterator ia = active.begin();
  for (vcl_vector<vgui_parent_child_link>::iterator i=children.begin() ; i!=children.end() ; ++i, ++ia)
    if ( (*i) == t ) {
      children.erase(i);
      active.erase(ia);
      notify();
      return true;
    }
  return false;
}


//----------------------------------------------------------------------------
//: Returns true if given integer could be an index to the child tableaux.
bool vgui_composite_tableau::index_ok(int v)
{
  return v >= 0 && v < int(children.size());
}


//----------------------------------------------------------------------------
bool vgui_composite_tableau::toggle(int v)
{
  if (!index_ok(v)) return false;
  bool state = active[v];
  active[v] = !state;

  return true;
}

//----------------------------------------------------------------------------
bool vgui_composite_tableau::is_active(int v)
{
  if (!index_ok(v)) return false;
  return active[v];
}
