// This is core/vgui/vgui_selector_tableau.cxx
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include "vgui_selector_tableau.h"
//:
// \file
// \author Matthew Leotta
// \date   November 5, 2003
// \brief  See vgui_selector_tableau.h for a description of this file.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_matrix_state.h>


//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_selector_tableau_new.
vgui_selector_tableau::vgui_selector_tableau()
{
}


//----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_selector_tableau_new.
// Many children, top to bottom.
vgui_selector_tableau::vgui_selector_tableau(std::vector<vgui_tableau_sptr> const& the_children)
{
  for (unsigned int i = 0; i < the_children.size(); ++i)
    add(the_children[i]);
  active_child_ = render_order_.front();
}

//----------------------------------------------------------------------------
//: Destructor - called by vgui_selector_tableau_sptr.
vgui_selector_tableau::~vgui_selector_tableau()
{
}

//----------------------------------------------------------------------------
//: Return the file name of the active tableau or just return the type.
std::string vgui_selector_tableau::file_name() const
{
  std::map<std::string, vgui_parent_child_link>::const_iterator itr = child_map_.find(active_child_);
  if (itr != child_map_.end())
    return itr->second->file_name();

  return type_name();
}


//----------------------------------------------------------------------------
//: Returns a nice version of the name, including info on the children.
std::string vgui_selector_tableau::pretty_name() const
{
  std::stringstream s; s << type_name() << "[#kids=" << child_map_.size() << ']';
  return s.str();
}

//----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool vgui_selector_tableau::handle(const vgui_event& event)
{
  // Save current matrix state. Each active child will be
  // invoked with this matrix state.
  vgui_matrix_state PM;

  // "DRAW" events. Return true unless some child returns false.
  if (event.type==vgui_DRAW || event.type==vgui_DRAW_OVERLAY) {
    bool retv = true;

    std::vector<std::string>::iterator itr = render_order_.begin();
    for (; itr != render_order_.end(); ++itr){
      if (visible_[*itr] && child_map_[*itr]) {
        PM.restore();
        if ( !child_map_[*itr]->handle(event) )
          retv=false;
      }
    }

    return retv;
  }

  // all other events :

  std::map<std::string, vgui_parent_child_link>::iterator itr = child_map_.find(active_child_);
  if (itr == child_map_.end() || !itr->second)
    return false;

  return itr->second->handle(event);
}

//----------------------------------------------------------------------------
//: Returns a bounding box large enough to contain all the visible child bounding boxes.
bool vgui_selector_tableau::get_bounding_box(float lo[3], float hi[3]) const
{
  // it would be nice if we could return an empty bounding box.
  if (child_map_.empty())
    return false;

  // get bbox of first visible child.
  std::vector<std::string>::const_iterator o_itr = render_order_.begin();
  for (; o_itr!=render_order_.end(); ++o_itr ){
    std::map<std::string, bool>::const_iterator v_itr = visible_.find(*o_itr);
    if ((v_itr != visible_.end()) && v_itr->second){
      std::map<std::string, vgui_parent_child_link>::const_iterator t_itr = child_map_.find(*o_itr);
      if (!t_itr->second->get_bounding_box(lo, hi))
        return false;
      break;
    }
  }
  // See if any children where visible
  if (o_itr == render_order_.end())
    return false;

  for (; o_itr!=render_order_.end(); ++o_itr ){
    std::map<std::string, bool>::const_iterator v_itr = visible_.find(*o_itr);
    if ((v_itr != visible_.end()) && v_itr->second){
      // get bbox of child *o_itr.
      std::map<std::string, vgui_parent_child_link>::const_iterator t_itr = child_map_.find(*o_itr);
      float l[3], h[3];
      if (!t_itr->second->get_bounding_box(l, h))
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
void vgui_selector_tableau::add(vgui_tableau_sptr const& tab, std::string name)
{
  if (name == "") name = tab->file_name();

  std::map<std::string, vgui_parent_child_link>::iterator itr = child_map_.find(name);
  bool exists = (itr != child_map_.end());

  child_map_[name] = vgui_parent_child_link(this,tab);
  if (!exists){
    render_order_.push_back(name);
    visible_[name] = true;
  }
  if (active_child_ == "") active_child_ = name;
}

//----------------------------------------------------------------------------
//: Add to list of child tableaux.
// virtual
bool vgui_selector_tableau::add_child(vgui_tableau_sptr const& t)
{
  this->add(t);
  return true;
}

//----------------------------------------------------------------------------
//: Remove given tableau from list of child tableaux.
void vgui_selector_tableau::remove(vgui_tableau_sptr const& t)
{
  if (!remove_child(t))
    std::cerr << __FILE__ " : no such child tableau\n";
}

//----------------------------------------------------------------------------
//: Remove given tableau from list of child tableaux.
bool vgui_selector_tableau::remove(const std::string name)
{
  std::map<std::string, vgui_parent_child_link>::iterator itr = child_map_.find(name);
  if (itr != child_map_.end())
    child_map_.erase(itr);
  else
    return false;

  std::map<std::string, bool>::iterator v_itr = visible_.find(name);
  if (v_itr != visible_.end())
    visible_.erase(v_itr);

  std::vector<std::string>::iterator o_itr = std::find(render_order_.begin(),
                                                    render_order_.end(),
                                                    name);
  if (o_itr != render_order_.end())
    render_order_.erase(o_itr);

  if (active_child_ == name){
    active_child_ = "";
    for (o_itr = render_order_.begin(); o_itr!=render_order_.end(); ++o_itr ){
      if (visible_[*o_itr]){
        active_child_ = *o_itr;
        break;
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------
void vgui_selector_tableau::clear()
{
  child_map_.clear();
  visible_.clear();
  render_order_.clear();
  active_child_ = "";
}

//----------------------------------------------------------------------------
//: Returns a smart pointer to the active tableau
vgui_tableau_sptr vgui_selector_tableau::active_tableau() const
{
  std::map<std::string, vgui_parent_child_link>::const_iterator itr = child_map_.find(active_child_);
  if (itr == child_map_.end()) return nullptr;
  return itr->second;
}

//----------------------------------------------------------------------------
//: Returns a smart pointer to the tableau with the given name
vgui_tableau_sptr vgui_selector_tableau::get_tableau(const std::string& name) const
{
  std::map<std::string, vgui_parent_child_link>::const_iterator itr = child_map_.find(name);
  if (itr == child_map_.end()) return nullptr;
  return itr->second;
}

//----------------------------------------------------------------------------
// virtual
bool vgui_selector_tableau::remove_child(vgui_tableau_sptr const &t)
{
  bool retval = false;

  for (std::map<std::string, vgui_parent_child_link>::iterator itr=child_map_.begin();
       itr!=child_map_.end(); ++itr )
    if ( (itr->second.child()) == t )
      retval = this->remove(itr->first) || retval;

  return retval;
}


//----------------------------------------------------------------------------
bool vgui_selector_tableau::toggle(const std::string& name)
{
  std::map<std::string, bool>::iterator itr = visible_.find(name);
  if (itr == visible_.end()) return false;
  bool state = itr->second;
  itr->second = !state;

  return true;
}

//----------------------------------------------------------------------------
bool vgui_selector_tableau::is_visible(const std::string& name) const
{
  std::map<std::string, bool>::const_iterator itr = visible_.find(name);
  if (itr == visible_.end()) return false;
  return itr->second;
}

//----------------------------------------------------------------------------
//: Make the child tableau with the given position the active child.
void vgui_selector_tableau::set_active(const std::string& name)
{
  std::map<std::string, vgui_parent_child_link>::iterator itr = child_map_.find(name);
  if (itr != child_map_.end()) active_child_ = name;
}

//----------------------------------------------------------------------------
//: Move the active tableau up one position in the display list.
void vgui_selector_tableau::active_raise()
{
  std::vector<std::string>::iterator itr;
  itr = std::find(render_order_.begin(), render_order_.end(), active_child_);
  if (itr != render_order_.end() && itr+1 != render_order_.end()) {
    *itr = *(itr+1);
    *(itr+1) = active_child_;
  }
}

//----------------------------------------------------------------------------
//: Move the active tableau down one position in the display list.
void vgui_selector_tableau::active_lower()
{
  std::vector<std::string>::iterator itr;
  itr = std::find(render_order_.begin(), render_order_.end(), active_child_);
  if (itr != render_order_.end() && itr != render_order_.begin()) {
    *itr = *(itr-1);
    *(itr-1) = active_child_;
  }
}

//----------------------------------------------------------------------------
//: Move the active tableau to the top of the display list.
void vgui_selector_tableau::active_to_top()
{
  std::vector<std::string>::iterator itr;
  itr = std::find(render_order_.begin(), render_order_.end(), active_child_);
  while (itr != render_order_.end() && itr+1 != render_order_.end()) {
    *itr = *(itr+1);
    ++itr;
  }
  *(itr) = active_child_;
}

//----------------------------------------------------------------------------
//: Move the active tableau to the bottom of the display list.
void vgui_selector_tableau::active_to_bottom()
{
  std::vector<std::string>::iterator itr;
  itr = std::find(render_order_.begin(), render_order_.end(), active_child_);
  while (itr != render_order_.end() && itr != render_order_.begin()) {
    *itr = *(itr-1);
    --itr;
  }
  *(itr) = active_child_;
}

//----------------------------------------------------------------------------
class vgui_selector_switch_command : public vgui_command
{
 public:
  vgui_selector_switch_command(vgui_selector_tableau* s, const std::string& n) : selector(s), name(n) {}
  void execute() { selector->set_active(name); selector->post_redraw(); }

  vgui_selector_tableau *selector;
  std::string name;
};

//----------------------------------------------------------------------------
class vgui_selector_toggle_command : public vgui_command
{
 public:
  vgui_selector_toggle_command(vgui_selector_tableau* s, const std::string& n) : selector(s), name(n) {}
  void execute() { selector->toggle(name); selector->post_redraw(); }

  vgui_selector_tableau *selector;
  std::string name;
};

//----------------------------------------------------------------------------
class vgui_selector_position_command : public vgui_command
{
 public:
  enum motion { TO_TOP, RAISE, LOWER, TO_BOTTOM };
  vgui_selector_position_command(vgui_selector_tableau* s, motion m) : selector(s), m_type(m) {}
  void execute()
  {
    switch (m_type)
    {
     case TO_TOP:
      selector->active_to_top();
      break;
     case RAISE:
      selector->active_raise();
      break;
     case LOWER:
      selector->active_lower();
      break;
     case TO_BOTTOM:
      selector->active_to_bottom();
      break;
     default: // should never be reached...
      break;
    };
    selector->post_redraw();
  }

  vgui_selector_tableau *selector;
  motion m_type;
};


//----------------------------------------------------------------------------
//: Builds a popup menu for the user to select the active child.
void vgui_selector_tableau::get_popup(const vgui_popup_params& params,
                                      vgui_menu &menu)
{
  vgui_menu submenu;

  // build child selection menus
  vgui_menu active_menu;
  vgui_menu visible_menu;
  vgui_menu position_menu;

  std::string check;
  std::vector<std::string>::reverse_iterator itr = render_order_.rbegin();
  for ( ; itr != render_order_.rend() ; ++itr) {
    std::string box_head, box_tail, check;
    if ( !get_tableau(*itr) ){
      box_head = "(";
      box_tail = ") ";
    }
    else{
      box_head = "[";
      box_tail = "] ";
    }

    if ( *itr == active_child_ ) check = "x";
    else check = " ";
    active_menu.add(box_head+check+box_tail+(*itr),
                    new vgui_selector_switch_command(this,*itr));

    if ( is_visible(*itr) ) check = "x";
    else check = " ";
    visible_menu.add(box_head+check+box_tail+(*itr),
                     new vgui_selector_toggle_command(this,*itr));
  }

  position_menu.add("Move to Top", new vgui_selector_position_command(this, vgui_selector_position_command::TO_TOP));
  position_menu.add("Move Up", new vgui_selector_position_command(this, vgui_selector_position_command::RAISE));
  position_menu.add("Move Down", new vgui_selector_position_command(this,vgui_selector_position_command::LOWER));
  position_menu.add("Move to Bottom", new vgui_selector_position_command(this,vgui_selector_position_command::TO_BOTTOM));

  submenu.add("Toggle Visible", visible_menu);
  submenu.add("Select Active", active_menu);
  submenu.add("Position Active", position_menu);
  add_to_menu(submenu);
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
