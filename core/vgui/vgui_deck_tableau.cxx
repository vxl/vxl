// This is oxl/vgui/vgui_deck_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   13 Sep 99
//-----------------------------------------------------------------------------

#include "vgui_deck_tableau.h"

#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>

#include <vgui/vgui_event.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_popup_params.h>

static bool debug=false;

vgui_deck_tableau::vgui_deck_tableau() : index_(-1) {}

vgui_deck_tableau::vgui_deck_tableau(vgui_tableau_sptr const& child0,
                     vgui_tableau_sptr const& child1) : index_(-1)
{
  add(child0);
  add(child1);
}

vgui_deck_tableau::vgui_deck_tableau(vgui_tableau_sptr const& child0,
                     vgui_tableau_sptr const& child1,
                     vgui_tableau_sptr const& child2) : index_(-1)
{
  add(child0);
  add(child1);
  add(child2);
}

vgui_deck_tableau::~vgui_deck_tableau() {}

bool vgui_deck_tableau::handle(const vgui_event& event) {

  if (vgui_tableau::handle(event))
    return true;

  if (!index_ok(index_))
    return false;

  vgui_tableau_sptr t = children[index_];
  return t->handle(event);
}

bool vgui_deck_tableau::help() {
  vcl_cerr << "\n"
           << "-- vgui_deck_tableau ----------------------------\n"
           << "|     keys                                      |\n"
           << "| `PageUp' and `PageDown'  switch current child |\n"
           << "-------------------------------------------------\n\n";

  return false;
}


bool vgui_deck_tableau::key_press(int x, int y, vgui_key key, vgui_modifier)
{
  if (debug)
    vcl_cerr << "vgui_deck_tableau::key_press " << key << vcl_endl;

  switch(key) {
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

void vgui_deck_tableau::add(vgui_tableau_sptr const& t)
{
  add_child(t);
}

// override virtual base class method
bool vgui_deck_tableau::add_child(vgui_tableau_sptr const& t)
{
  children.push_back( vgui_slot(this,t) );
  index_ = size()-1;
  observers.notify();
  post_redraw();
  return true;
}

void vgui_deck_tableau::remove(vgui_tableau_sptr const& t)
{
  if (!remove_child(t))
    vcl_cerr << __FILE__ " no such child tableau : " << t << vcl_endl;
}

// override virtual base class  method
bool vgui_deck_tableau::remove_child(vgui_tableau_sptr const& t)
{
  for (vcl_vector<vgui_slot>::iterator i = children.begin() ; i!=children.end() ; ++i)
    if ( (*i) == t ) {
      children.erase(i);
      // Index must be allowed to go to -1 if all are deleted, so don't check for zero
      --index_;
      observers.notify();
      return true;
    }
  return false;
}


vgui_tableau_sptr vgui_deck_tableau::current()
{
  if (index_ok(index_))
    return children[index_];

  return 0;
}

vgui_tableau_sptr vgui_deck_tableau::get_tableau_at(int tab_pos)
{
  if (index_ok(tab_pos))
    return children[tab_pos];
  return 0;
}

int vgui_deck_tableau::size()
{
  return children.size();
}

void vgui_deck_tableau::index(int v)
{
  if (index_ok(v)) index_ = v;
  if (debug)
    vcl_cerr << "vgui_deck_tableau::index " << index_ << vcl_endl;
  observers.notify();
}

void vgui_deck_tableau::begin()
{
  if (index_ok(0))
    index_ = 0;

  if (debug)
    vcl_cerr << "vgui_deck_tableau::begin " << index_ << vcl_endl;
  observers.notify();
}

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

bool vgui_deck_tableau::index_ok(int v) const
{
  return v >= 0 && v < int(children.size());
}

vcl_string vgui_deck_tableau::type_name() const { return "vgui_deck_tableau"; }

vcl_string vgui_deck_tableau::file_name() const
{
  if (index_ok(index_)) {
    return children[index_]->file_name();
  }
  return type_name();
}


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


class vgui_deck_switch_command : public vgui_command
{
 public:
  vgui_deck_switch_command(vgui_deck_tableau* d, int i) : deck(d), index(i) {}
  void execute() { deck->index(index); deck->post_redraw(); }

  vgui_deck_tableau *deck;
  int index;
};


void vgui_deck_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu) {

  vgui_menu submenu;

  // build autogenerated items
#if 0 // commented out
  if (params.defaults) {

    vcl_string active_label("Toggle active ");
    if (active_) active_label+="[on]";
    else active_label+="[off]";

    submenu.add(active_label.c_str(), new vgui_command_simple<vgui_tableau>(this, &vgui_tableau::toggle_active));


    vcl_string visible_label("Toggle visible ");
    if (visible_) visible_label+="[on]";
    else visible_label+="[off]";

    submenu.add(visible_label.c_str(), new vgui_command_simple<vgui_tableau>(this, &vgui_tableau::toggle_visible));
  }
#endif // end commented out

  // build child selection menu
  vgui_menu selections;

  int count = 0;
  vcl_vector<vgui_slot>::iterator i = children.begin();
  for ( ; i!=children.end() ; ++i, ++count) {
    selections.add((*i)->file_name().c_str(), new vgui_deck_switch_command(this,count));
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
