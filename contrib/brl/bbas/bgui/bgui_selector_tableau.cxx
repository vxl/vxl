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
{
}


//----------------------------------------------------------------------------
//: Constructor - don't use this, use bgui_selector_tableau_new.
// Many children, top to bottom.
bgui_selector_tableau::bgui_selector_tableau(vcl_vector<vgui_tableau_sptr> const& the_children)
{
  for (unsigned int i = 0; i < the_children.size(); ++i)
    add(the_children[i]);
  active_child_ = render_order_.front();
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
  vcl_map<vcl_string, vgui_parent_child_link>::const_iterator itr = child_map_.find(active_child_);
  if (itr != child_map_.end())
    return itr->second->file_name();
    
  return type_name();
}


//----------------------------------------------------------------------------
//: Returns a nice version of the name, including info on the children.
vcl_string bgui_selector_tableau::pretty_name() const
{
  vcl_stringstream s; s << type_name() << "[#kids=" << child_map_.size() << ']';
  return s.str();
}

//----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool bgui_selector_tableau::handle(const vgui_event& event)
{
  // Save current matrix state. Each active child will be
  // invoked with this matrix state.
  vgui_matrix_state PM;

  // "DRAW" events. Return true unless some child returns false.
  if (event.type==vgui_DRAW || event.type==vgui_DRAW_OVERLAY) {
    bool retv = true;

    vcl_vector<vcl_string>::iterator itr = render_order_.begin();
    for (; itr != render_order_.end(); ++itr){
      if (visible_[*itr]) {
        PM.restore();
        if ( !child_map_[*itr]->handle(event) )
          retv=false;
      }
    }

    return retv;
  }

  // all other events :

  vcl_map<vcl_string, vgui_parent_child_link>::iterator itr = child_map_.find(active_child_);
  if (itr == child_map_.end())
    return false;
    
  return itr->second->handle(event);
}

//----------------------------------------------------------------------------
//: Returns a bounding box large enough to contain all the visible child bounding boxes.
bool bgui_selector_tableau::get_bounding_box(float lo[3], float hi[3]) const
{
  // it would be nice if we could return an empty bounding box.
  if (child_map_.empty())
    return false;

  // get bbox of first visible child.
  vcl_vector<vcl_string>::const_iterator o_itr = render_order_.begin();
  for (; o_itr!=render_order_.end(); ++o_itr ){
    vcl_map<vcl_string, bool>::const_iterator v_itr = visible_.find(*o_itr);
    if ((v_itr != visible_.end()) && v_itr->second){
      vcl_map<vcl_string, vgui_parent_child_link>::const_iterator t_itr = child_map_.find(*o_itr);
      if (!t_itr->second->get_bounding_box(lo, hi))
        return false;
      break;
    }
  }
  // See if any children where visible
  if(o_itr == render_order_.end())
    return false;

  for (; o_itr!=render_order_.end(); ++o_itr ){
    vcl_map<vcl_string, bool>::const_iterator v_itr = visible_.find(*o_itr);
    if ((v_itr != visible_.end()) && v_itr->second){
      // get bbox of child *o_itr.
      vcl_map<vcl_string, vgui_parent_child_link>::const_iterator t_itr = child_map_.find(*o_itr);
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
void bgui_selector_tableau::add(vgui_tableau_sptr const& tab, vcl_string name)
{
  if(name == "") name = tab->file_name();
  
  vcl_map<vcl_string, vgui_parent_child_link>::iterator itr = child_map_.find(name);
  bool exists = (itr != child_map_.end());
  
  child_map_[name] = vgui_parent_child_link(this,tab);
  if(!exists){
    render_order_.push_back(name);
    visible_[name] = true;
  }
  if(active_child_ == "") active_child_ = name;
}

//----------------------------------------------------------------------------
//: Add to list of child tableaux.
// virtual
bool bgui_selector_tableau::add_child(vgui_tableau_sptr const& t)
{
  this->add(t);
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
//: Remove given tableau from list of child tableaux.
bool bgui_selector_tableau::remove(const vcl_string& name)
{
  vcl_map<vcl_string, vgui_parent_child_link>::iterator itr = child_map_.find(name);
  if (itr != child_map_.end())
    return remove_child(itr->second.child());
  return false;
}

//----------------------------------------------------------------------------
void bgui_selector_tableau::clear()
{
  child_map_.clear();
  visible_.clear();
  render_order_.clear();
  active_child_ = "";
}

//----------------------------------------------------------------------------
//: Returns a smart pointer to the active tableau
vgui_tableau_sptr bgui_selector_tableau::active_tableau()
{
  vcl_map<vcl_string, vgui_parent_child_link>::iterator itr = child_map_.find(active_child_);
  if (itr == child_map_.end()) return NULL;
  return itr->second;
}

//----------------------------------------------------------------------------
// virtual
bool bgui_selector_tableau::remove_child(vgui_tableau_sptr const &t)
{
  for( vcl_map<vcl_string, vgui_parent_child_link>::iterator itr=child_map_.begin();
       itr!=child_map_.end(); ++itr )
    if ( (itr->second.child()) == t ) {
      vcl_string name = itr->first;
      child_map_.erase(itr);
      
      vcl_map<vcl_string, bool>::iterator v_itr = visible_.find(name);
      if(v_itr != visible_.end())
        visible_.erase(v_itr);
        
      for( vcl_vector<vcl_string>::iterator o_itr = render_order_.begin();
           o_itr!=render_order_.end(); ++o_itr ){
        if (name == *o_itr)
          render_order_.erase(o_itr--);
      }
      
      if (active_child_ == name){
        active_child_ = "";
        for( vcl_vector<vcl_string>::iterator o_itr = render_order_.begin();
             o_itr!=render_order_.end(); ++o_itr ){
          if(visible_[*o_itr]){
            active_child_ = *o_itr;
            break;
          }
        }
      }
      return true;
    }
  return false;
}


//----------------------------------------------------------------------------
bool bgui_selector_tableau::toggle(const vcl_string& name)
{
  vcl_map<vcl_string, bool>::iterator itr = visible_.find(name);
  if (itr == visible_.end()) return false;
  bool state = itr->second;
  itr->second = !state;

  return true;
}

//----------------------------------------------------------------------------
bool bgui_selector_tableau::is_visible(const vcl_string& name) const
{
  vcl_map<vcl_string, bool>::const_iterator itr = visible_.find(name);
  if (itr == visible_.end()) return false;
  return itr->second;
}

//----------------------------------------------------------------------------
//: Make the child tableau with the given position the active child.
void bgui_selector_tableau::set_active(const vcl_string& name)
{
  vcl_map<vcl_string, vgui_parent_child_link>::iterator itr = child_map_.find(name);
  if (itr != child_map_.end()) active_child_ = name;
}

//----------------------------------------------------------------------------
class bgui_selector_switch_command : public vgui_command
{
 public:
  bgui_selector_switch_command(bgui_selector_tableau* s, const vcl_string& n) : selector(s), name(n) {}
  void execute() { selector->set_active(name); selector->post_redraw(); }

  bgui_selector_tableau *selector;
  vcl_string name;
};

//----------------------------------------------------------------------------
class bgui_selector_toggle_command : public vgui_command
{
 public:
  bgui_selector_toggle_command(bgui_selector_tableau* s, const vcl_string& n) : selector(s), name(n) {}
  void execute() { selector->toggle(name); selector->post_redraw(); }

  bgui_selector_tableau *selector;
  vcl_string name;
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
  vcl_vector<vcl_string>::iterator itr = render_order_.begin();
  for ( ; itr != render_order_.end() ; ++itr) {
    if( *itr == active_child_ ) check = "[x] ";
    else check = "[ ] ";
    active_menu.add(check+(*itr), new bgui_selector_switch_command(this,*itr));

    if( is_visible(*itr) ) check = "[x] ";
    else check = "[ ] ";
    visible_menu.add(check+(*itr), new bgui_selector_toggle_command(this,*itr));
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

