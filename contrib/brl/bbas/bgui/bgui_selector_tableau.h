// This is brl/bbas/bgui/bgui_selector_tableau.h
#ifndef bgui_selector_tableau_h_
#define bgui_selector_tableau_h_
//:
// \file
// \brief  Tableau that allows the selection of one active child but displays all children
// \author Matthew Leotta (mleotta@brown.lems.edu)
// \date   11-5-2003
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_map.h>

#include <bgui/bgui_selector_tableau_sptr.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_observable.h>

class vgui_event;

//: Tableau that allows the selection of one active child but displays all children
//
//  The bgui_selector_tableau class can have any number of children, indexed
//  from 0 upwards.  The draw action of bgui_selector_tableau is to draw each
//  of its children, in order, into  the current context if they are marked visible.
//  Events reaching the bgui_selector_tableau are passed on to the active child only.
//
//  The exceptions to this rule are :
//  [a] the DRAW, DRAW_OVERLAY events which are sent to all children.
class bgui_selector_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use bgui_selector_tableau_new.
  //  Creates an empty composite tableau.
  bgui_selector_tableau();

  //: Constructor - don't use this, use bgui_selector_tableau_new.
  //  Takes a vector of child tableaux.
  bgui_selector_tableau(vcl_vector<vgui_tableau_sptr> const& children);

  //: Handle all events sent to this tableau.
  //  Key-press '?' prints info on this file, before being sent to the children.
  virtual bool handle(const vgui_event&);

  //: Returns the type of this tableau ('bgui_selector_tableau').
  vcl_string type_name() const;

  //: There is no obvious filename, so this just returns the type.
  vcl_string file_name() const;

  //: Returns a nice version of the name, including info on the children.
  vcl_string pretty_name() const;

  //: Builds a popup menu for the user to select the active child and set visibility.
  //  Over-rides function in vgui_tableau.
  virtual void get_popup(const vgui_popup_params&, vgui_menu &m);

  //: Add a tableau to the list of child tableaux.
  void add(vgui_tableau_sptr const& tab, vcl_string name = "");

  //: Remove a tableau from the list of child tableaux.
  void remove(vgui_tableau_sptr const& tab);

  //: Remove a tableau from the list of child tableaux by name.
  bool remove(const vcl_string& name);

  //: Clear the list of child tableaux.
  void clear();

  //: Returns a smart pointer to the active tableau
  vgui_tableau_sptr active_tableau();

  //: Returns the name of the active tableau
  const vcl_string& active_name() const { return active_child_; }

  //: Returns a smart pointer to the tableau with the given name
  vgui_tableau_sptr get_tableau(const vcl_string& name);
  
  //: Make the child tableau with the given name the active child.
  void set_active(const vcl_string& name);

  //: Toggle the child tableau with the given name between visible/invisible.
  bool toggle(const vcl_string& name);

  //: Returns true if the child tableau with the given name is active.
  bool is_visible(const vcl_string& name) const;

  //: Returns the number of children
  int num_children() const { return child_map_.size(); }

  //: Returns a vector containing the names of all children (in rendering order)
  const vcl_vector<vcl_string>& child_names() const { return render_order_; }

 protected:
  //: Destructor - called by bgui_selector_tableau_sptr.
  virtual ~bgui_selector_tableau();

  //: Returns a bounding box large enough to contain all child bounding boxes.
  bool get_bounding_box(float low[3], float high[3]) const;

  //: Add to list of child tableaux.
  bool add_child(vgui_tableau_sptr const& t);

  //: Remove given tableau from list of child tableaux.
  bool remove_child(vgui_tableau_sptr const& );

  // data
  // ----

  //: Whether each child is visible or not (ie. using events).
  vcl_map<vcl_string, bool> visible_;

  //: The unique child names sorted in rendering order
  vcl_vector<vcl_string> render_order_;

  //: A map from unique names to children
  vcl_map<vcl_string, vgui_parent_child_link> child_map_;

  //: The name of the active tableau
  vcl_string active_child_;
};

//: Creates a smart-pointer to a bgui_selector_tableau tableau.
struct bgui_selector_tableau_new : public bgui_selector_tableau_sptr
{
  typedef bgui_selector_tableau_sptr base;

  //: Constructor - creates a pointer to an empty bgui_selector_tableau.
  bgui_selector_tableau_new() : base(new bgui_selector_tableau()) { }


  //: Constructor - creates pointer to a composite with the given children.
  //  Takes a vector of child tableaux.
  bgui_selector_tableau_new(vcl_vector<vgui_tableau_sptr> const& children)
    : base(new bgui_selector_tableau(children)) {}
};

#endif // bgui_selector_tableau_h_
