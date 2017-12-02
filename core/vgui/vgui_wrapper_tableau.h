// This is core/vgui/vgui_wrapper_tableau.h
#ifndef vgui_wrapper_tableau_h_
#define vgui_wrapper_tableau_h_
//:
// \file
// \author fsm
// \brief  Base class tableau for tableau who want only a single child.
//
//  Contains classes: vgui_wrapper_tableau  vgui_wrapper_tableau_new

#include "vgui_wrapper_tableau_sptr.h"
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>

//: Base class tableau for tableau who want only a single child.
//
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
class vgui_wrapper_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_wrapper_tableau_new.
  //  The child tableau is added later using add_child.
  vgui_wrapper_tableau();

  //: Constructor - don't use this, use vgui_wrapper_tableau_new.
  //  Takes the single child tableau for this tableau.
  vgui_wrapper_tableau(vgui_tableau_sptr const&);

  //: Adds given tableau as child if none exists, else causes error.
  bool add_child(vgui_tableau_sptr const&);

  //: The child tableau is removed if it is the same as the given tableau.
  bool remove_child(vgui_tableau_sptr const&);

  //: Returns the child's file_name if it exists.
  std::string file_name() const;

  //: Returns nice version of the name which also includes details of the child.
  std::string pretty_name() const;

  //: Returns the type of tableau ('vgui_wrapper_tableau').
  std::string type_name() const;

  //: Handle all events sent to this tableau.
  //  It is pointless to derive from vgui_wrapper_tableau
  //  unless this method is also overridden!
  bool handle(vgui_event const &);

  //: Get the bounding box for this tableau.
  //  Defaults to getting the bounding box of the child.
  bool get_bounding_box(float low[3], float high[3]) const;

  //: The single child of this tableau.
  vgui_parent_child_link child;

 protected:
  // Destructor - called by vgui_wrapper_tableau_sptr.
  ~vgui_wrapper_tableau();
};

//: Create a smart-pointer to a vgui_wrapper_tableau.
struct vgui_wrapper_tableau_new : public vgui_wrapper_tableau_sptr {
  typedef vgui_wrapper_tableau_sptr base;

  //: Constructor - creates a default vgui_wrapper_tableau.
  vgui_wrapper_tableau_new() : base(new vgui_wrapper_tableau()) { }

  //: Constructor - takes the single child for the vgui_wrapper_tableau.
  vgui_wrapper_tableau_new(vgui_tableau_sptr const&b) : base(new vgui_wrapper_tableau(b)) { }
};

#endif // vgui_wrapper_tableau_h_
