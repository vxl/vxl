// This is oxl/vgui/vgui_composite.h
#ifndef vgui_composite_h_
#define vgui_composite_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   15 Sep 99
// \brief  Tableau which treats it children as a stack of acetates
//
// \verbatim
//  Modifications:
//   18 Sep 00 capes@robots. Added set_enable_key_bindings. Key bindings are OFF by default.
//    9 Feb 01 awf@robots. Add Alt-C to re-enable key bindings.
//   26-APR-2002 K.Y.McGaul - Converted to doxygen style comments.
// \endverbatim

#include <vcl_vector.h>

#include <vgui/vgui_composite_sptr.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_observable.h>

class vgui_event;

//: Tableau which treats it children as a stack of acetates
//
//  The vgui_composite class can have any number of children, indexed from 
//  0 upwards.  The draw action of vgui_composite is to draw each of its 
//  children, in order, into  the current context.  Events reaching the 
//  vgui_composite are passed on to each child in turn, till it is handled, 
//  so that child 0, the first added, is the "top" tableau.
//
//  The exceptions to this rule are :
//  [a] key presses '0'-'9', which toggle the activeness of the children and
//  [b] the DRAW, DRAW_OVERLAY events which are sent to all children.
class vgui_composite : public vgui_tableau
{
public:
  //: Constructor - don't use this, use vgui_composite_new.
  //  Creates an empty composite tableau.
  vgui_composite();

  //: Constructor - don't use this, use vgui_composite_new.
  //  Creates a composite with two child tableaux.
  vgui_composite(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1);

  //: Constructor - don't use this, use vgui_composite_new.
  //  Creates a composite with three child tableaux.
  vgui_composite(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1, vgui_tableau_sptr const& child2);

  //: Constructor - don't use this, use vgui_composite_new.
  //  Takes a list of child tableaux.
  vgui_composite(vcl_vector<vgui_tableau_sptr> const& children);

  //: Handle all events sent to this tableau.
  //  All events (except key-presses '0'-'9' and draw events) are passed 
  //  to each child until the event is handled.
  //  Key presses '0'-'9' toggle the activeness of the children and
  //  draw events are sent to all children.
  //  Key-press '?' prints info on this file, before being sent to the children.
  virtual bool handle(const vgui_event&);

  //: Prints info about this tableau - called when '?'is pressed.
  virtual bool help();

  //: Returns the type of this tableau ('vgui_composite').
  vcl_string type_name() const;

  //: Returns the type????
  vcl_string file_name() const;

  //: Returns a nice version of the name, including info on the children.
  vcl_string pretty_name() const;

  //: Calls notify for the observers.
  virtual void notify() const;

  //: Conceptually, this is a list on which observers can put themselves.
  vgui_observable observers;

  //: Add a tableau to the list of child tableaux.
  void add(vgui_tableau_sptr const&);

  //: Remove a tableau from the list of child tableux.
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
  //  Defaut is Alt-C enables / disables key bindings
  vgui_event_condition c_enable_key_bindings;

 protected:
  virtual ~vgui_composite();

  bool get_bounding_box(float low[3], float high[3]) const;
  bool add_child(vgui_tableau_sptr const& t);
  bool remove_child(vgui_tableau_sptr const& );

  // helper
  bool index_ok(int);

  // data

  //: List of child tableaux.
  vcl_vector<vgui_slot> children;
  vcl_vector<bool> active;
  bool enable_key_bindings;
};

//: Creates a smart-pointer to a vgui_composite tableau.
struct vgui_composite_new : public vgui_composite_sptr
{
  typedef vgui_composite_sptr base;
  vgui_composite_new() : base(new vgui_composite()) { }
  vgui_composite_new(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1)
    : base(new vgui_composite(child0, child1)) { }
  vgui_composite_new(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1, vgui_tableau_sptr const& child2)
    : base(new vgui_composite(child0, child1, child2)) { }
  vgui_composite_new(vcl_vector<vgui_tableau_sptr> const& children): base(new vgui_composite(children)) {}
};

#endif // vgui_composite_h_
