// This is core/vgui/vgui_deck_tableau.h
#ifndef vgui_deck_tableau_h_
#define vgui_deck_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief  Tableau holding many child tableaux, but only one receives events.
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   13 Sep 99
//
//  Contains classes  vgui_deck_tableau  vgui_deck_tableau_new
//
// \verbatim
//  Modifications
//   13-SEP-1999 P.Pritchett - Initial version.
//   26-APR-2002 K.Y.McGaul - Converted to and added doxygen style comments.
// \endverbatim

#include "vgui_deck_tableau_sptr.h"
#include <vgui/vgui_observable.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vcl_string.h>

//: Tableau holding many child tableaux, but only one receives events.
//
//  vgui_deck_tableau holds an ordered collection of child tableaux, only one
//  of which is passed all events that the vgui_deck_tableau receives. The
//  effect is a flick-book of tableaux where the currently active tableau can
//  be changed using PageUp and PageDown
class vgui_deck_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_deck_tableau_new.
  //  Make an empty deck
  vgui_deck_tableau() : index_(-1) {}

  //: Constructor - don't use this, use vgui_deck_tableau_new.
  //  Make a deck with two children, listed top to bottom
  vgui_deck_tableau(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1);

  //: Constructor - don't use this, use vgui_deck_tableau_new.
  //  Make a deck with three children, listed top to bottom
  vgui_deck_tableau(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1, vgui_tableau_sptr const& child2);

  //: Add a tableau to the deck
  //  It is placed on top, and made current.
  void add(vgui_tableau_sptr const& t) { add_child(t); }

  //: Remove the tableau pointed to by P.
  //  The one below is then made current.
  void remove(vgui_tableau_sptr const& p);

  //: Return a pointer to the current tableau
  vgui_tableau_sptr current();

  //: Return a pointer to the tableau at a given location
  vgui_tableau_sptr get_tableau_at(int);

  //: Return number of child tableaux in the deck.
  int size();

  //: Say which tableau is current
  int index() const {return index_;}

  //: Make a particular tableau current.
  void index(int);

  //: Make the top tableau current
  void begin();

  //: Make the next tableau down the list current
  void next();

  //: Make the next higher tableau current
  void prev();

  //: Returns the file_name of the active child.
  //  Over-rides function in vgui_tableau.
  virtual vcl_string file_name() const;

  //: Returns a nice version of the name, including info on the active child.
  //  Over-rides function in vgui_tableau.
  virtual vcl_string pretty_name() const;

  //: Returns the type of this tableau ('vgui_deck_tableau').
  //  Over-rides function in vgui_tableau.
  virtual vcl_string type_name() const;

  //: Builds a popup menu for the user to select the active child.
  //  Over-rides function in vgui_tableau.
  virtual void get_popup(const vgui_popup_params&, vgui_menu &m);

  //: Send info to cerr - called when user presses '?' in the rendering area.
  //  Over-rides function in vgui_tableau.
  //  This function is called by the default handle() function in vgui_tableau.
  bool help();

  //: Uses PageUp and PageDown events - called when user presses a key.
  //  Over-rides function in vgui_tableau.
  //  This function is called by the default handle() function in vgui_tableau.
  bool key_press(int x, int y, vgui_key key, vgui_modifier);

  //: Conceptually, this is a list on which observers can put themselves.
  vgui_observable observers;

 protected:
  //: Destructor - called by vgui_deck_tableau_sptr.
  virtual ~vgui_deck_tableau();

  //: Handle events by passing to the current child tableau.
  virtual bool handle(const vgui_event&);

  //: Add a tableau to the deck
  //  It is placed on top, and made current.
  //  Overrides virtual base class method.
  bool add_child(vgui_tableau_sptr const& t);

  //: Remove the given child tableau from the deck.
  bool remove_child(vgui_tableau_sptr const& );

  //: Returns true if given integer could be an index to the list of children.
  bool index_ok(int) const;

  // data
  //-----

  //: List of child tableaux.
  vcl_vector<vgui_parent_child_link> children;

  //: Currently active child tableau.
  int index_;
};

//: Create a smart-pointer to a vgui_deck_tableau.
struct vgui_deck_tableau_new : public vgui_deck_tableau_sptr
{
  typedef vgui_deck_tableau_sptr base;

  //: Constructor - creates a pointer to an empty vgui_deck_tableau.
  vgui_deck_tableau_new() : base(new vgui_deck_tableau()) {}

  //: Constructor - creates a pointer to a vgui_deck_tableau with two children.
  //  Children are given top to bottom.
  vgui_deck_tableau_new(vgui_tableau_sptr const& child0,vgui_tableau_sptr const& child1)
    : base(new vgui_deck_tableau(child0, child1)) {}

  //: Constructor - creates a pointer to a vgui_deck_tableau with 3 children.
  //  Children are given top to bottom.
  vgui_deck_tableau_new(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1, vgui_tableau_sptr const& child2)
    : base(new vgui_deck_tableau(child0, child1, child2)) {}
};

#endif // vgui_deck_tableau_h_
