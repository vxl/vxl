// This is core/vgui/vgui_composite_tableau.h
#ifndef vgui_composite_tableau_h_
#define vgui_composite_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau which treats it children as a stack of acetates
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   15 Sep 99
//
// \verbatim
//  Modifications
//   18-Sep-2000 capes@robots - Added set_enable_key_bindings.
//                              Key bindings are OFF by default.
//   09-Feb-2001 awf@robots   - Add Alt-C to re-enable key bindings.
//   26-APR-2002 K.Y.McGaul   - Converted to doxygen style comments.
//   01-OCT-2002 K.Y.McGaul   - Moved vgui_composite to vgui_composite_tableau.
// \endverbatim

#include <vcl_vector.h>

#include <vgui/vgui_composite_tableau_sptr.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_observable.h>

class vgui_event;

//: Tableau which treats it children as a stack of acetates
//
//  The vgui_composite_tableau class can have any number of children, indexed
//  from 0 upwards.  The draw action of vgui_composite_tableau is to draw each
//  of its children, in order, into  the current context.  Events reaching the
//  vgui_composite_tableau are passed on to each child in turn, till it is
//  handled, so that child 0, the first added, is the "top" tableau.
//
//  The exceptions to this rule are :
//  [a] key presses '0'-'9', which toggle the activeness of the children and
//  [b] the DRAW, DRAW_OVERLAY events which are sent to all children.
class vgui_composite_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_composite_tableau_new.
  //  Creates an empty composite tableau.
  vgui_composite_tableau();

  //: Constructor - don't use this, use vgui_composite_tableau_new.
  //  Creates a composite with two child tableaux.
  vgui_composite_tableau(vgui_tableau_sptr const& child0,
                         vgui_tableau_sptr const& child1);

  //: Constructor - don't use this, use vgui_composite_tableau_new.
  //  Creates a composite with three child tableaux.
  vgui_composite_tableau(vgui_tableau_sptr const& child0,
                         vgui_tableau_sptr const& child1,
                         vgui_tableau_sptr const& child2);

  //: Constructor - don't use this, use vgui_composite_tableau_new.
  //  Takes a vector of child tableaux.
  vgui_composite_tableau(vcl_vector<vgui_tableau_sptr> const& children);

  //: Handle all events sent to this tableau.
  //  All events (except key-presses '0'-'9' and draw events) are passed
  //  to each child until the event is handled.
  //  Key presses '0'-'9' toggle the activeness of the children and
  //  draw events are sent to all children.
  //  Key-press '?' prints info on this file, before being sent to the children.
  virtual bool handle(const vgui_event&);

  //: Prints info about this tableau - called when '?'is pressed.
  virtual bool help();

  //: Returns the type of this tableau ('vgui_composite_tableau').
  vcl_string type_name() const { return "vgui_composite_tableau"; }

  //: There is no obvious filename, so this just returns the type.
  vcl_string file_name() const { return type_name(); }

  //: Returns a nice version of the name, including info on the children.
  vcl_string pretty_name() const;

  //: Calls notify for the observers.
  virtual void notify() const;

  //: Conceptually, this is a list on which observers can put themselves.
  vgui_observable observers;

  //: Add a tableau to the list of child tableaux.
  //  Same as add_child().
  void add(vgui_tableau_sptr const& t) { add_child(t); }

  //: Remove a tableau from the list of child tableaux.
  void remove(vgui_tableau_sptr const&);

  //: Clear the list of child tableaux.
  void clear();

  //: Toggle the child tableau in the given position between active/inactive.
  bool toggle(int);

  //: Returns true if the child tableau in the given position is active.
  bool is_active(int);

  //: Enable (if true) or disable (if false) key bindings.
  //  If key bindings is enabled then key presses '0'-'9' will be used
  //  by this tableau to toggle the activeness of child tableaux.
  void set_enable_key_bindings(bool on) { enable_key_bindings = on; }

  //: The event which occurs to toggle enable/disable key bindings.
  //  Default is Alt-C enables / disables key bindings
  vgui_event_condition c_enable_key_bindings;

 protected:
  //: Destructor - called by vgui_composite_tableau_sptr.
  virtual ~vgui_composite_tableau() {}

  //: Returns a bounding box large enough to contain all child bounding boxes.
  bool get_bounding_box(float low[3], float high[3]) const;

  //: Add to list of child tableaux.
  bool add_child(vgui_tableau_sptr const& t);

  //: Remove given tableau from list of child tableaux.
  bool remove_child(vgui_tableau_sptr const& );

  //: Returns true if the given integer could be an index to the child tableaux.
  bool index_ok(int);

  // data
  // ----

  //: List of child tableaux.
  vcl_vector<vgui_parent_child_link> children;

  //: Whether each child is active or not (ie. using events).
  vcl_vector<bool> active;

  bool enable_key_bindings;
};

//: Creates a smart-pointer to a vgui_composite_tableau tableau.
struct vgui_composite_tableau_new : public vgui_composite_tableau_sptr
{
  typedef vgui_composite_tableau_sptr base;

  //: Constructor - creates a pointer to an empty vgui_composite_tableau.
  vgui_composite_tableau_new() : base(new vgui_composite_tableau()) {}

  //: Constructor - creates a pointer to a composite with two children.
  vgui_composite_tableau_new(vgui_tableau_sptr const& child0,
                             vgui_tableau_sptr const& child1)
    : base(new vgui_composite_tableau(child0, child1)) {}

  //: Constructor - creates a pointer to a composite with three children.
  vgui_composite_tableau_new(vgui_tableau_sptr const& child0,
                             vgui_tableau_sptr const& child1,
                             vgui_tableau_sptr const& child2)
    : base(new vgui_composite_tableau(child0, child1, child2)) {}

  //: Constructor - creates pointer to a composite with the given children.
  //  Takes a vector of child tableaux.
  vgui_composite_tableau_new(vcl_vector<vgui_tableau_sptr> const& children)
    : base(new vgui_composite_tableau(children)) {}
};

#endif // vgui_composite_tableau_h_
