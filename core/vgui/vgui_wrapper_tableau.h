#ifndef vgui_wrapper_tableau_h_
#define vgui_wrapper_tableau_h_

// .NAME vgui_wrapper_tableau
// .INCLUDE vgui/vgui_wrapper_tableau.h
// .FILE vgui_wrapper_tableau.cxx
//
// .SECTION Description
// Q: So what does a vgui_wrapper_tableau do, then?
// A: It's a convenient base class for tableaux who want only a single child,
//    providing mixin code for adding and removing children, and handling the
//    popup menu.
//    To pass on an event to (the child of) a slot use
//      return child.handle(e);
//    which will work even if the slot is empty (returns false).
//
// Q: Why does it have such a silly name?
// A: Because it is a tableau which "wraps" itself around another tableau.
//    "vgui_parent" was too vague and it was needed for something else anyway.
//    I would welcome suggestions for a better name.
//
// @author fsm@robots.ox.ac.uk

#include "vgui_wrapper_tableau_sptr.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>

class vgui_wrapper_tableau : public vgui_tableau {
public:
  vgui_wrapper_tableau();
  vgui_wrapper_tableau(vgui_tableau_sptr const&);

  bool add_child(vgui_tableau_sptr const&);
  bool remove_child(vgui_tableau_sptr const&);

  vcl_string file_name() const;
  vcl_string pretty_name() const;
  vcl_string type_name() const;

  // It is pointless to derive from vgui_wrapper_tableau
  // unless this method is also overridden!
  bool handle(vgui_event const &);

  // defaults to gettting the bounding box of the child.
  bool get_bounding_box(float low[3], float high[3]) const;

  vgui_slot child;
protected:
  ~vgui_wrapper_tableau();
};

struct vgui_wrapper_tableau_new : public vgui_wrapper_tableau_sptr {
  typedef vgui_wrapper_tableau_sptr base;
  vgui_wrapper_tableau_new() : base(new vgui_wrapper_tableau()) { }
  vgui_wrapper_tableau_new(vgui_tableau_sptr const&b) : base(new vgui_wrapper_tableau(b)) { }
};

#endif // vgui_wrapper_tableau_h_
