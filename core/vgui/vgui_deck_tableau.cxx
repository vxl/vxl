// This is core/vgui/vgui_deck_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   13 Sep 99
// \brief  See vgui_deck_tableau.h for a description of this file.

#include "vgui_deck_tableau.h"

#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_popup_params.h>

static bool debug=false;

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_deck_tableau_new.
//  Makes an empty deck
vgui_deck_tableau::vgui_deck_tableau() : index_(-1)
{
}

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_deck_tableau_new.
//  Make a deck with two children, listed top to bottom.
vgui_deck_tableau::vgui_deck_tableau(vgui_tableau_sptr const& child0,
                                     vgui_tableau_sptr const& child1) : index_(-1)
{
  add(child0);
  add(child1);
}

//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_deck_tableau_new.
//  Make a deck with three children, listed top to bottom.
vgui_deck_tableau::vgui_deck_tableau(vgui_tableau_sptr const& child0,
                                     vgui_tableau_sptr const& child1,
                                     vgui_tableau_sptr const& child2) : index_(-1)
{
  add(child0);
  add(child1);
  add(child2);
}

//----------------------------------------------------------------------------
//: Destructor - called by vgui_deck_tableau_sptr.
vgui_deck_tableau::~vgui_deck_tableau() {}

//----------------------------------------------------------------------------
//: Handle events by passing to the current child tableau.
bool vgui_deck_tableau::handle(const vgui_event& event)
{
  // First pass events to the default handle method (this calls key_press, etc).
  if (vgui_tableau::handle(event))
    return true;

  if (!index_ok(index_))
    return false;

  // Then pass events to the current child tableau.
  vgui_tableau_sptr t = children[index_];
  return t->handle(event);
}

//----------------------------------------------------------------------------
//: Send info to cerr - called when user presses '?' in the rendering area.
//  Over-rides function in vgui_tableau.
//  This function is called by the default handle() function in vgui_tableau.
bool vgui_deck_tableau::help()
{
  vcl_cerr << "\n-- vgui_deck_tableau ----------------------------\n"
           << "|     keys                                      |\n"
           << "| `PageUp' and `PageDown'  switch current child |\n"
           << "-------------------------------------------------\n\n";

  return false;
}

//----------------------------------------------------------------------------
//: Uses PageUp and PageDown events - called when user presses a key.
//  Over-rides function in vgui_tableau.
//  This function is called by the default handle() function in vgui_tableau.
bool vgui_deck_tableau::key_press(int x, int y, vgui_key key, vgui_modifier)
{
  if (debug)
    vcl_cerr << "vgui_deck_tableau::key_press " << key << vcl_endl;

  switch (key)
  {
   case vgui_PAGE_UP:
    this->next();
    if (index_ok(index_) && children[index_]) {
      vgui_event e(vgui_ENTER);
      e.wx = x;
      e.wy = y;
      children[index_]->handle(e);
    }
    this->post_redraw();
    return true;
   case vgui_PAGE_DOWN:
    this->prev();
    if (index_ok(index_) && children[index_]) {
      vgui_event e(vgui_ENTER);
      e.wx = x;
      e.wy = y;
      children[index_]->handle(e);
    }
    this->post_redraw();
    return true;
   default:
    return false;
  }
}

//----------------------------------------------------------------------------
//: Add a tableau to the deck.
//  It is placed on top and made current.
void vgui_deck_tableau::add(vgui_tableau_sptr const& t)
{
  add_child(t);
}

//----------------------------------------------------------------------------
//: Add a tableau to the deck.
//  It is placed on top and made current.
//  Override virtual base class method.
bool vgui_deck_tableau::add_child(vgui_tableau_sptr const& t)
{
  children.push_back( vgui_parent_child_link(this,t) );
  index_ = size()-1;
  observers.notify();
  post_redraw();
  return true;
}

//----------------------------------------------------------------------------
//: Remove the given child tableau from the deck.
void vgui_deck_tableau::remove(vgui_tableau_sptr const& t)
{
  if (!remove_child(t))
    vcl_cerr << __FILE__ " no such child tableau : " << t << vcl_endl;
}

//----------------------------------------------------------------------------
//: Remove the given child tableau from the deck.
//  Override virtual base class  method
bool vgui_deck_tableau::remove_child(vgui_tableau_sptr const& t)
{
  for (vcl_vector<vgui_parent_child_link>::iterator i = children.begin() ; i!=children.end() ; ++i)
    if ( (*i) == t ) {
      children.erase(i);
      // Index must be allowed to go to -1 if all are deleted, so don't
      // check for zero
      --index_;
      observers.notify();
      return true;
    }
  return false;
}


//----------------------------------------------------------------------------
//: Returns the index of the currently active child tableau.
vgui_tableau_sptr vgui_deck_tableau::current()
{
  if (index_ok(index_))
    return children[index_];

  return 0;
}

//----------------------------------------------------------------------------
//: Returns the tableau at the given index in the list of child tableau.
vgui_tableau_sptr vgui_deck_tableau::get_tableau_at(int tab_pos)
{
  if (index_ok(tab_pos))
    return children[tab_pos];
  return 0;
}

//----------------------------------------------------------------------------
//: Returns the number of child tableaux.
int vgui_deck_tableau::size()
{
  return children.size();
}

//----------------------------------------------------------------------------
//: Make the child tableau with the given position the active child.
void vgui_deck_tableau::index(int v)
{
  if (index_ok(v)) index_ = v;
  if (debug)
    vcl_cerr << "vgui_deck_tableau::index " << index_ << vcl_endl;
  observers.notify();
}

//----------------------------------------------------------------------------
//: Make the top tableau current.
void vgui_deck_tableau::begin()
{
  if (index_ok(0))
    index_ = 0;

  if (debug)
    vcl_cerr << "vgui_deck_tableau::begin " << index_ << vcl_endl;
  observers.notify();
}

//----------------------------------------------------------------------------
//: Make the next tableau down the list current.
void vgui_deck_tableau::next()
{
  unsigned int tmp = index_;

  if (tmp+1 >= children.size())
    tmp=0;
  else
    ++tmp;

  if (index_ok(tmp))
    index_=tmp;

  if (debug)
    vcl_cerr << "vgui_deck_tableau::next " << index_ << vcl_endl;
  observers.notify();
}

//----------------------------------------------------------------------------
//: Make the next higher tableau current.
void vgui_deck_tableau::prev()
{
  int tmp = index_;

  if (tmp == 0)
    tmp=children.size()-1;
  else
    --tmp;

  if (index_ok(tmp))
    index_=tmp;

  if (debug)
    vcl_cerr << "vgui_deck_tableau::prev " << index_ << vcl_endl;
  observers.notify();
}

//----------------------------------------------------------------------------
//: Returns true if the given integer could be an index to the list of children.
bool vgui_deck_tableau::index_ok(int v) const
{
  return v >= 0 && v < int(children.size());
}

//----------------------------------------------------------------------------
//: Returns the type of this tableau ('vgui_deck_tableau').
vcl_string vgui_deck_tableau::type_name() const { return "vgui_deck_tableau"; }

//----------------------------------------------------------------------------
//: Returns the filename of the currently active child tableau.
vcl_string vgui_deck_tableau::file_name() const
{
  if (index_ok(index_)) {
    return children[index_]->file_name();
  }
  return type_name();
}

//----------------------------------------------------------------------------
//: Returns a nice version of the name, with info on the currently active child.
vcl_string vgui_deck_tableau::pretty_name() const
{
  vcl_string name;
  if (index_ok(index_)) {
    name += "[current = ";
    name += children[index_]->pretty_name();
    name += "]";
  }
  return name;
}

//----------------------------------------------------------------------------
class vgui_deck_switch_command : public vgui_command
{
 public:
  vgui_deck_switch_command(vgui_deck_tableau* d, int i) : deck(d), index(i) {}
  void execute() { deck->index(index); deck->post_redraw(); }

  vgui_deck_tableau *deck;
  int index;
};


//----------------------------------------------------------------------------
//: Builds a popup menu for the user to select the active child.
void vgui_deck_tableau::get_popup(const vgui_popup_params& params,
                                  vgui_menu &menu)
{
  vgui_menu submenu;

  // build child selection menu
  vgui_menu selections;

  int count = 0;
  vcl_vector<vgui_parent_child_link>::iterator i = children.begin();
  for ( ; i!=children.end() ; ++i, ++count) {
    selections.add((*i)->file_name().c_str(),
                   new vgui_deck_switch_command(this,count));
  }

  submenu.add("Select active child", selections);

  if (params.nested)
  {
    // nested menu style

    vgui_tableau_sptr c = current();

    if (c && params.defaults) submenu.separator();
    c->get_popup(params, submenu);
    menu.add(type_name(), submenu);
  }
  else
  {
    menu.add(type_name(),submenu);

    if (params.recurse) {
      vgui_tableau_sptr c = current();
      if (c)
        c->get_popup(params, menu);
    }
  }
}
