// This is core/vgui/vgui_poly_tableau.h
#ifndef vgui_poly_tableau_h_
#define vgui_poly_tableau_h_
//:
// \file
// \brief  Tableau which renders its children in sub-rectangles of its viewport.
// \author fsm
//
//  Contains classes vgui_poly_tableau  vgui_poly_tableau_new
//
// \verbatim
//  Modifications
//   01-OCT-2002 K.Y.McGaul - Combined vgui_polytab_base with vgui_polytab.
//                          - Added some Doxygen style comments.
//                          - Moved vgui_polytab to vgui_poly_tableau.
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>

#include "vgui_poly_tableau_sptr.h"

//: A tableau which renders its children into sub-rectangles of its viewport.
//
// Class poly_tableau is a tableau which renders its children into
// sub-rectangles of its given viewport. The subrectangles are given as
// relative coordinates on [0,1]x[0,1], with (0,0) being the lower left corner
// and (1,1) the upper right corner.
//
// vgui_poly_tableau has a concept of which child is 'current', meaning
// roughly which child is getting the mouse events. It automatically
// switches current child, according to where the pointer is, in a
// sensible way.
//
// vgui_poly_tableau can be used to emulate two adaptors side by side.
//
// Implementation notes:
// Many methods take an argument "GLint const vp[4]", which is the viewport (in
// the format returned by OpenGL) as it was when the last event reached the
// tableau. For example, it is not possible to switch 'current' child without
// knowing the viewport, because a LEAVE/ENTER pair have to be sent to the old
// and new child and the viewport must be set correctly before dispatching these
// events.

//-----------------------------------------------------------------------------
//: Viewport helper class
//  The constructor takes a snapshot of the current viewport and scissor areas.
//  The destructor restores that state.
class vgui_poly_tableau_vp_sc_snapshot
{
 public:
  GLint vp[4];
  GLint sc[4];
  bool sc_was_enabled;

  vgui_poly_tableau_vp_sc_snapshot() {
    glGetIntegerv(GL_VIEWPORT, vp);

    glGetIntegerv(GL_SCISSOR_BOX, sc);
    sc_was_enabled = glIsEnabled(GL_SCISSOR_TEST) == GL_TRUE;
  }

  ~vgui_poly_tableau_vp_sc_snapshot() {
    // restore viewport :
    glViewport(vp[0], vp[1], vp[2], vp[3]);

    // turn off the scissor test, if it wasn't already on, and
    // restore old scissor settings :
    if (sc_was_enabled)
      glEnable(GL_SCISSOR_TEST);
    else
      glDisable(GL_SCISSOR_TEST);
    glScissor(sc[0], sc[1], sc[2], sc[3]);
  }
};

class vgui_poly_tableau : public vgui_tableau
{
 public:
  //: Constructor - don't use this, use vgui_poly_tableau_new.
  vgui_poly_tableau();

  //: Returns the type of this tableau ('vgui_poly_tableau').
  vcl_string type_name() const;

  //: Get popup menu.
  void get_popup(vgui_popup_params const &, vgui_menu &);

  //: The position, colour, etc of the child tableau.
  struct item
  {
    vgui_parent_child_link tab;
    float x,y,w,h;
    int outline_color[3];
    int id;

    item() { } // for stl container
    item(vgui_tableau* p, vgui_tableau_sptr const&c, float x, float y,
         float w, float h, int id =0);
    void set_vp(GLint const vp[4]);
    //: Returns true if the given position is inside the boundaries of this item
    bool inside(GLint const vp[4], int x, int y) const;
  };

  typedef vcl_vector<item> container;
  typedef container::iterator iterator;
  typedef container::const_iterator const_iterator;

  //: Returns the number of items in the list of items.
  unsigned size() const { return sub.size(); }

  //: Return an iterator pointing to the first item in the list of items.
  iterator begin() { return sub.begin(); }

  //: Return a const iterator pointing to the first item in the list of items.
  const_iterator begin() const { return sub.begin(); }

  //: Return an iterator pointing to the last item in the list of items.
  iterator end() { return sub.end(); }

  //: Return a const iterator pointing to the last item in the list of items.
  const_iterator end() const { return sub.end(); }

  //: Erase the item at the given position from the list of items.
  void erase(iterator );

  //: Adds the given tableau to the given proportion of the viewport.
  //  x,y,w,h specify a portion of the vgui_poly_tableau's viewport in
  //  coordinates which go from 0 to 1.
  //  Returns handle to child.
  int add(vgui_tableau_sptr const&, float x, float y, float w, float h);

  //: Remove subtableau, referred to by handle.
  void remove(int id);

  //: Move subtableau to a new location.
  void move(int id, float x, float y, float w, float h);

  //: Replace the tableau with the given ID, with the given tableau.
  //  Keep the same ID and do not change the value of 'current'.
  void replace(int id, vgui_tableau_sptr const& tab);

  //: Get pointer to tableau from id.
  vgui_tableau_sptr get(int id) const;

  //: Set color to outline tableau.
  void set_outline_color(const int id, const int r, const int g, const int b);

 protected:
  //: Destructor - called by vgui_poly_tableau_sptr.
  ~vgui_poly_tableau();

  //: Handle all events sent to this tableau.
  //  In particular, use draw events to draw the sub-rectangles.
  bool handle(vgui_event const &);

  //; Make sure draw events go to all children in the right order.
  bool handle(GLint const vp[4], vgui_event const &e);

  //: Misnomer - returns the index of child under the pointer's position.
  int get_active(GLint const vp[4], int wx, int wy) const;

  int get_current() const { return current; }
  int get_current_id();
  void set_current(GLint const vp[4], int index);

  //: Index of the item currently getting events.
  int current;

  //: List of items displayed by this tableau.
  vcl_vector<item> sub;

  bool may_switch_child;
};


//: Creates a smart-pointer to a vgui_poly_tableau tableau.
struct vgui_poly_tableau_new : public vgui_poly_tableau_sptr
{
  typedef vgui_poly_tableau_sptr base;

  //: Constructor - create a smart-pointer to an empty vgui_poly_tableau.
  vgui_poly_tableau_new() : base(new vgui_poly_tableau()) { }
};

#endif // vgui_poly_tableau_h_
