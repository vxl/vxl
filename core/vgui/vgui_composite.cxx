//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation
#endif
//
// Class: vgui_composite
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 15 Sep 99
//
//-----------------------------------------------------------------------------


#include "vgui_composite.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vbl/vbl_sprintf.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_projection_inspector.h>

//quell_warning static bool debug=false;

bool vgui_composite::help() {
  vcl_cerr << vcl_endl;
  vcl_cerr << "-- vgui_composite --------------" << vcl_endl;
  vcl_cerr << "|     keys                     |" << vcl_endl;
  vcl_cerr << "| `1' to `9'  toggle child `n' |" << vcl_endl;
  vcl_cerr << "--------------------------------" << vcl_endl;
  vcl_cerr << vcl_endl;
  return false;
}

vgui_composite::vgui_composite()
{
  enable_key_bindings = false;
}

// -- Constructor taking two children.   The first is on top, the second below.
vgui_composite::vgui_composite(vgui_tableau_ref const& child0, vgui_tableau_ref const& child1)
{
  add(child0);
  add(child1);
  enable_key_bindings = false;
}

// -- Three children, top to bottom.
vgui_composite::vgui_composite(vgui_tableau_ref const& child0, vgui_tableau_ref const& child1, vgui_tableau_ref const& child2)
{
  add(child0);
  add(child1);
  add(child2);
  enable_key_bindings = false;
}

// -- Many children, top to bottom.
vgui_composite::vgui_composite(vcl_vector<vgui_tableau_ref> const& the_children)
{
  for(int i = 0; i < the_children.size(); ++i)
    add(the_children[i]);
  enable_key_bindings = false;
}

vgui_composite::~vgui_composite()
{
}

vcl_string vgui_composite::type_name() const
{
  return "vgui_composite";
}

vcl_string vgui_composite::file_name() const
{
  return type_name();
}


vcl_string vgui_composite::pretty_name() const
{
  return vbl_sprintf("%s[#kids=%d]", type_name().c_str(), children.size());
}

bool vgui_composite::handle(const vgui_event& event)
{
  // Save current matrix state. Each active child will be 
  // invoked with this matrix state. 
  vgui_matrix_state PM;
  int n = children.size();

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

void vgui_composite::notify() const {
  observers./*vgui_tableau::*/notify();
/*
  for (vcl_vector<vgui_slot>::const_iterator t_iter = children.begin();
       t_iter != children.end(); ++t_iter) {
    vgui_tableau_ref t = *t_iter;
    if(t)
      t->notify();
  }
*/
}

bool vgui_composite::get_bounding_box(float lo[3], float hi[3]) const {
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

void vgui_composite::add(vgui_tableau_ref const& t) {
  add_child(t);
}

// virtual
bool vgui_composite::add_child(vgui_tableau_ref const& t) {
  children.push_back( vgui_slot(this,t) );
  active.push_back(true);
  notify();
  return true;
}

void vgui_composite::remove(vgui_tableau_ref const& t) {
  if (!remove_child(t))
    vcl_cerr << __FILE__ " : no such child tableau" << vcl_endl;
}

// virtual
bool vgui_composite::remove_child(vgui_tableau_ref const &t) {
  vcl_vector<bool>::iterator ia = active.begin();
  for (vcl_vector<vgui_slot>::iterator i=children.begin() ; i!=children.end() ; ++i, ++ia)
    if ( (*i) == t ) {
      children.erase(i);
      active.erase(ia);
      notify();
      return true;
    }
  return false;
}


bool vgui_composite::index_ok(int v) {
  if (v < 0 || v >= children.size())
    return false;

  return true;
}


bool vgui_composite::toggle(int v) {
  if (!index_ok(v)) return false;
  bool state = active[v];
  active[v] = !state;

  return true;
}

bool vgui_composite::is_active(int v) {
  if (!index_ok(v)) return false;
  return active[v];
}

