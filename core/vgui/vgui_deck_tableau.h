// This is oxl/vgui/vgui_deck_tableau.h
#ifndef vgui_deck_tableau_h_
#define vgui_deck_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Philip C. Pritchett, Robotics Research Group, University of Oxford
// \date   13 Sep 99
// \brief  Tableau which holds many child tableau, only one of which receives events.
//
// \verbatim
//  Modifications:
//    13-SEP-1999 P.Pritchett - Initial version.
//    26-APR-2002 K.Y.McGaul - Converted to and added doxygen style comments.
// \endverbatim

#include "vgui_deck_tableau_sptr.h"
#include <vgui/vgui_observable.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
#include <vcl_string.h>

//: Tableau which holds many child tableau, only one of which receives events.
//
//  vgui_deck_tableau holds an ordered collection of child tableaux, only one of which is
//  passed all events that the vgui_deck_tableau receives. The effect is a flick-book of
//  tableaux where the currently active tableau can be changed using PageUp and PageDown
class vgui_deck_tableau : public vgui_tableau
{
 public:
  //: Make an empty deck
  vgui_deck_tableau();

  //: Make a deck with two children, listed top to bottom
  vgui_deck_tableau(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1);

  //: Make a deck with three children, listed top to bottom
  vgui_deck_tableau(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1, vgui_tableau_sptr const& child2);

  // vgui_deck_tableau methods

  //: Add a tableau to the deck
  // It is placed on top, and made current.
  void add(vgui_tableau_sptr const&);

  //: Remove the tableau pointed to by P.
  // The one below is then mde current.
  void remove(vgui_tableau_sptr const& p);

  //: Return a pointer to the current tableau
  vgui_tableau_sptr current();

  //: Return a pointer to the tableau at a given location
  vgui_tableau_sptr get_tableau_at(int);

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

  //: conceptually, this is a list on which observers can put themselves.
  vgui_observable observers;

 protected:
  virtual ~vgui_deck_tableau();
  virtual bool handle(const vgui_event&);

  bool add_child(vgui_tableau_sptr const& t);
  bool remove_child(vgui_tableau_sptr const& );

  // helper
  bool index_ok(int) const;

  // data
  vcl_vector<vgui_slot> children;
  int index_;
};

struct vgui_deck_tableau_new : public vgui_deck_tableau_sptr
{
  typedef vgui_deck_tableau_sptr base;
  vgui_deck_tableau_new() : base(new vgui_deck_tableau()) { }
  vgui_deck_tableau_new(vgui_tableau_sptr const& child0,vgui_tableau_sptr const& child1)
    : base(new vgui_deck_tableau(child0, child1)) { }
  vgui_deck_tableau_new(vgui_tableau_sptr const& child0, vgui_tableau_sptr const& child1, vgui_tableau_sptr const& child2)
    : base(new vgui_deck_tableau(child0, child1, child2)) { }
};

#endif // vgui_deck_tableau_h_
