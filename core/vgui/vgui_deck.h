#ifndef vgui_deck_h_
#define vgui_deck_h_
#ifdef __GNUC__
#pragma interface
#endif
// 
// .NAME vgui_deck - A deck of tableaux
// .LIBRARY vgui
// .INCLUDE vgui/vgui_deck.h
// .FILE vgui_deck.cxx
//
// .SECTION Description
//
// vgui_deck holds an ordered collection of child tableaux, only one of which is
// passed all events that the vgui_deck receives. The effect is a flick-book of 
// tableaux where the currently active tableau can be changed using PageUp and PageDown
//
// .SECTION Author
//              Philip C. Pritchett, 13 Sep 99
//              Robotics Research Group, University of Oxford
//
// .SECTION Modifications
//
//-----------------------------------------------------------------------------

#include "vgui_deck_ref.h"
#include <vgui/vgui_observable.h>
#include <vgui/vgui_drag_mixin.h>
#include <vgui/vgui_slot.h>

class vgui_deck : public vgui_tableau {
public:
  //: Make an empty deck
  vgui_deck();

  //: Make a deck with two children, listed top to bottom
  vgui_deck(vgui_tableau_ref const& child0, vgui_tableau_ref const& child1);

  //: Make a deck with three children, listed top to bottom
  vgui_deck(vgui_tableau_ref const& child0, vgui_tableau_ref const& child1, vgui_tableau_ref const& child2);

  // vgui_deck methods

  //: Add a tableau to the deck
  // It is placed on top, and made current.
  void add(vgui_tableau_ref const&);

  //: Remove the tableau pointed to by P.
  // The one below is then mde current.
  void remove(vgui_tableau_ref const& p);

  //: Return a pointer to the current tableau
  vgui_tableau_ref current();

  //: Return a pointer to the tableau at a given location
  vgui_tableau_ref get_tableau_at(int);

  //: Return number of tableaux on deck.
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

  // vgui_tableau methods
  virtual vcl_string file_name() const;
  virtual vcl_string pretty_name() const;
  virtual vcl_string type_name() const;

  virtual void get_popup(const vgui_popup_params&, vgui_menu &m);

  // Event handling
  bool help();
  bool key_press(int x, int y, vgui_key key, vgui_modifier);

  // conceptually, this is a list on which observers can put themselves.
  vgui_observable observers;

protected:
  virtual ~vgui_deck();
  virtual bool handle(const vgui_event&);

  bool add_child(vgui_tableau_ref const& t);
  bool remove_child(vgui_tableau_ref const& );

  // helper
  bool index_ok(int) const;
  
  // data
  vcl_vector<vgui_slot> children;
  int index_;
};

struct vgui_deck_new : public vgui_deck_ref {
  typedef vgui_deck_ref base;
  vgui_deck_new() : base(new vgui_deck()) { }
  vgui_deck_new(vgui_tableau_ref const& child0, vgui_tableau_ref const& child1) : base(new vgui_deck(child0, child1)) { }
  vgui_deck_new(vgui_tableau_ref const& child0, vgui_tableau_ref const& child1, vgui_tableau_ref const& child2) : base(new vgui_deck(child0, child1, child2)) { }
};

#endif // vgui_deck_h_
