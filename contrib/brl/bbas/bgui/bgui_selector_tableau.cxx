// This is core/bgui/bgui_selector_tableau.cxx

//:
// \file
// \author Matthew Leotta
// \date   11-5-2003
// \brief  See bgui_selector_tableau.h for a description of this file.


#include <bgui/bgui_selector_tableau.h>

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_matrix_state.h>


//----------------------------------------------------------------------------
//: Constructor - don't use this, use bgui_selector_tableau_new.
bgui_selector_tableau::bgui_selector_tableau()
  : active_index_(-1)
{
}


//----------------------------------------------------------------------------
//: Constructor - don't use this, use bgui_selector_tableau_new.
// Many children, top to bottom.
bgui_selector_tableau::bgui_selector_tableau(vcl_vector<vgui_tableau_sptr> const& the_children)
{
  for (unsigned int i = 0; i < the_children.size(); ++i)
    add(the_children[i]);
  active_index_ = the_children.size()-1;
}

//----------------------------------------------------------------------------
//: Destructor - called by bgui_selector_tableau_sptr.
bgui_selector_tableau::~bgui_selector_tableau()
{
}

//----------------------------------------------------------------------------
//: Returns the type of this tableau ('bgui_selector_tableau').
vcl_string bgui_selector_tableau::type_name() const
{
  return "bgui_selector_tableau";
}

//----------------------------------------------------------------------------
//: Return the file name of the active tableau or just return the type.
vcl_string bgui_selector_tableau::file_name() const
{
  if (index_ok(active_index_))
    return children_[active_index_]->file_name();
    
  return type_name();
}


//----------------------------------------------------------------------------
//: Returns a nice version of the name, including info on the children.
vcl_string bgui_selector_tableau::pretty_name() const
{
  vcl_stringstream s; s << type_name() << "[#kids=" << children_.size() << ']';
  return s.str();
}

//----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool bgui_selector_tableau::handle(const vgui_event& event)
{
  // Save current matrix state. Each active child will be
  // invoked with this matrix state.
  vgui_matrix_state PM;
  unsigned int n = children_.size();

  // "DRAW" events. Return true unless some child returns false.
  if (event.type==vgui_DRAW || event.type==vgui_DRAW_OVERLAY) {
    bool retv = true;

    for (unsigned ia = 0; ia<n; ++ia) {
      if (visible_[ia] && children_[ia]) {
        PM.restore();
        if ( !children_[ia]->handle(event) )
          retv=false;
      }
    }

    return retv;
  }

  // all other events :
  
  if(!index_ok(active_index_))
    return false;
    
  return children_[active_index_]->handle(event);
}

//----------------------------------------------------------------------------
//: Returns a bounding box large enough to contain all the visible child bounding boxes.
bool bgui_selector_tableau::get_bounding_box(float lo[3], float hi[3]) const
{
  // it would be nice if we could return an empty bounding box.
  if (children_.empty())
    return false;

  // get bbox of first visible child.
  unsigned first;
  for (first=0; first<children_.size(); ++first) {
    if (visible_[first]){
      if (!children_[first]->get_bounding_box(lo, hi))
        return false;
      else break;
    }
  }
  // See if any children where visible
  if(first >= children_.size())
    return false;
  
  for (unsigned i=first+1; i<children_.size(); ++i) {
    if (visible_[i]){
      // get bbox of child i.
      float l[3], h[3];
      if (!children_[i]->get_bounding_box(l, h))
        return false;

      // enlarge if necessary.
      for (unsigned j=0; j<3; ++j) {
        if (l[j] < lo[j]) lo[j] = l[j];
        if (h[j] > hi[j]) hi[j] = h[j];
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------
//: Same as add_child().
void bgui_selector_tableau::add(vgui_tableau_sptr const& t, vcl_string name)
{
  add_child(t);
  if(name == "") name = t->file_name();
  menu_names_.push_back(name);
}

//----------------------------------------------------------------------------
//: Add to list of child tableaux.
// virtual
bool bgui_selector_tableau::add_child(vgui_tableau_sptr const& t)
{
  children_.push_back( vgui_parent_child_link(this,t) );
  visible_.push_back(true);
  active_index_ = children_.size()-1;
  return true;
}

//----------------------------------------------------------------------------
//: Remove given tableau from list of child tableaux.
void bgui_selector_tableau::remove(vgui_tableau_sptr const& t)
{
  if (!remove_child(t))
    vcl_cerr << __FILE__ " : no such child tableau\n";
}

//----------------------------------------------------------------------------
void bgui_selector_tableau::clear()
{
  children_.clear();
  visible_.clear();
  active_index_ = -1;
}

//----------------------------------------------------------------------------
//: Returns a smart pointer to the active tableau
vgui_tableau_sptr bgui_selector_tableau::active_tableau()
{
  if (index_ok(active_index_))
    return children_[active_index_];

  return NULL;
}

//----------------------------------------------------------------------------
// virtual
bool bgui_selector_tableau::remove_child(vgui_tableau_sptr const &t)
{
  vcl_vector<bool>::iterator iv = visible_.begin();
  vcl_vector<vcl_string>::iterator in = menu_names_.begin();
  int c=0;
  for( vcl_vector<vgui_parent_child_link>::iterator i=children_.begin();
       i!=children_.end(); ++i, ++iv, ++in, ++c )
    if ( (*i) == t ) {
      children_.erase(i);
      visible_.erase(iv);
      menu_names_.erase(in);
      if (active_index_ >= c)
        --active_index_;
      return true;
    }
  return false;
}


//----------------------------------------------------------------------------
//: Returns true if given integer could be an index to the child tableaux.
bool bgui_selector_tableau::index_ok(int v) const
{
  return v >= 0 && v < int(children_.size());
}


//----------------------------------------------------------------------------
bool bgui_selector_tableau::toggle(int v)
{
  if (!index_ok(v)) return false;
  bool state = visible_[v];
  visible_[v] = !state;

  return true;
}

//----------------------------------------------------------------------------
bool bgui_selector_tableau::is_visible(int v)
{
  if (!index_ok(v)) return false;
  return visible_[v];
}

//----------------------------------------------------------------------------
//: Make the child tableau with the given position the active child.
void bgui_selector_tableau::set_active(int i)
{
  if (index_ok(i)) active_index_ = i;
}

//----------------------------------------------------------------------------
class bgui_selector_switch_command : public vgui_command
{
 public:
  bgui_selector_switch_command(bgui_selector_tableau* s, int i) : selector(s), index(i) {}
  void execute() { selector->set_active(index); selector->post_redraw(); }

  bgui_selector_tableau *selector;
  int index;
};

//----------------------------------------------------------------------------
class bgui_selector_toggle_command : public vgui_command
{
 public:
  bgui_selector_toggle_command(bgui_selector_tableau* s, int i) : selector(s), index(i) {}
  void execute() { selector->toggle(index); selector->post_redraw(); }

  bgui_selector_tableau *selector;
  int index;
};


//----------------------------------------------------------------------------
//: Builds a popup menu for the user to select the active child.
void bgui_selector_tableau::get_popup(const vgui_popup_params& params,
                                            vgui_menu &menu)
{
  vgui_menu submenu;

  // build child selection menus
  vgui_menu active_menu;
  vgui_menu visible_menu;

  vcl_string check;
  int count = 0;
  vcl_vector<vcl_string>::iterator i = menu_names_.begin();
  for ( ; i!=menu_names_.end() ; ++i, ++count) {
    if( count == active_index_ ) check = "[x] ";
    else check = "[ ] ";
    active_menu.add(check+i->c_str(), new bgui_selector_switch_command(this,count));

    if( is_visible(count) ) check = "[x] ";
    else check = "[ ] ";
    visible_menu.add(check+i->c_str(), new bgui_selector_toggle_command(this,count));
  }

  submenu.add("Select Active", active_menu);
  submenu.add("Toggle Visible", visible_menu);

  if (params.nested)
  {
    // nested menu style

    vgui_tableau_sptr a = active_tableau();

    if (a && params.defaults) submenu.separator();
    a->get_popup(params, submenu);
    menu.add(type_name(), submenu);
  }
  else
  {
    menu.add(type_name(),submenu);

    if (params.recurse) {
      vgui_tableau_sptr a = active_tableau();
      if (a)
        a->get_popup(params, menu);
    }
  }
}

