// This is ./oxl/vgui/vgui_polytab.h

//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  Tableau which renders its children in sub-rectangles of its viewport.
//
//  Contains classes: vgui_polytab  vgui_polytab_new  vgui_polytab_base

#ifndef vgui_polytab_h_
#define vgui_polytab_h_

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>

//: Base class for vgui_polytab - don't use this, use vgui_polytab.
//
// polytab_base is a tableau which renders its children into sub-rectangles
// of its given viewport. The subrectangles are given as relative coordinates
// on [0,1]x[0,1], with (0,0) being the lower left corner and (1,1) the upper
// right corner. polytab_base has a concept of which child is 'current', meaning
// roughly which child is getting the mouse events.
class vgui_polytab_base : public vgui_tableau {
public:
  vgui_polytab_base();

  vcl_string type_name() const;

  // "iterator interface"
  struct item {
    vgui_slot tab;
    float x,y,w,h;
    int outline_color[3];
    int id;

    item() { } // for stl container
    item(vgui_tableau* p, vgui_tableau_sptr const&c, float x, float y, float w, float h, int id =0);
    void set_vp(GLint const vp[4]);
    bool inside(GLint const vp[4], int x, int y) const;
  };
  typedef vcl_vector<item> container;
  typedef container::iterator iterator;
  typedef container::const_iterator const_iterator;
  unsigned size() const { return sub.size(); }
  iterator begin() { return sub.begin(); }
  const_iterator begin() const { return sub.begin(); }
  iterator end() { return sub.end(); }
  const_iterator end() const { return sub.end(); }
  void erase(iterator );

  // "handle interface"

  //: Add new subtableau, returns handle to child.
  int add(vgui_tableau_sptr const&, float x, float y, float w, float h);
  //: Remove subtableau, referred to by handle.
  void remove(int id);
  //: Move subtableau.
  void move(int id, float x, float y, float w, float h);
  //:
  void replace(int id, vgui_tableau_sptr const& tab);
  //: Get pointer to tableau from id.
  vgui_tableau_sptr get(int id) const;
  //: Set color to outline tableau.
  void set_outline_color(const int id, const int r, const int g, const int b);

protected:
  ~vgui_polytab_base();
  bool handle(vgui_event const &);
  bool handle(GLint const vp[4], vgui_event const &e);

  // misnomer. returns the index of the child currently under
  // the pointer's position.
  int get_active(GLint const vp[4], int wx, int wy) const;

  int get_current() const { return current; }
  int get_current_id();
  void set_current(GLint const vp[4], int index);

  //private:
  int current; // index of item currently getting events.
  vcl_vector<item> sub;
};

#include "vgui_polytab_sptr.h"

//: A tableau which renders its children into sub-rectangles of its viewport.
//
// Class polytab_base is a tableau which renders its children into 
// sub-rectangles of its given viewport. The subrectangles are given as 
// relative coordinates on [0,1]x[0,1], with (0,0) being the lower left corner 
// and (1,1) the upper right corner. polytab_base has a concept of which child 
// is 'current', meaning roughly which child is getting the mouse events.
//
// Class polytab is derived from polytab_base and automatically switches current
// child, according to where the pointer is, in a sensible way.
//
// This can be used to emulate two adaptors side by side.
//
// Implementation notes:
// Many methods take an argument "GLint const vp[4]", which is the viewport (in
// the format returned by OpenGL) as it was when the last event reached the
// tableau. For example, it is not possible to switch 'current' child without
// knowing the viewport, because a LEAVE/ENTER pair have to be sent to the old
// and new child and the viewport must be set correctly before dispatching these
// events.
class vgui_polytab : public vgui_polytab_base {
public:
  //: Constructor - don't use this, use vgui_polytab_new.
  vgui_polytab();

  //: Returns the type of this tableau ('vgui_polytab').
  vcl_string type_name() const;

  void get_popup(vgui_popup_params const &, vgui_menu &);

protected:
  ~vgui_polytab();
  bool may_switch_child;
  bool handle(vgui_event const &);
};

//: Creates a smart-pointer to a vgui_polytab tableau.
struct vgui_polytab_new : public vgui_polytab_sptr {
  typedef vgui_polytab_sptr base;
  vgui_polytab_new() : base(new vgui_polytab()) { }
};

#endif // vgui_polytab_h_
