//:
// \file
//
// Class vgui_poly_tableau is a tableau which renders its children into sub-rectangles
// of its given viewport. The subrectangles are given as relative coordinates
// on [0,1]x[0,1], with (0,0) being the lower left corner and (1,1) the upper
// right corner. vgui_poly_tableau has a concept of which child is 'current', meaning
// roughly which child is getting the mouse events.
//
// Class vgui_poly_tableau is derived from vgui_poly_tableau and automatically switches current
// child, according to where the pointer is, in a sensible way.
//
// This can be used to emulate two adaptors side by side.

// \par Implementation notes:
// Many methods take an argument "GLint const vp[4]", which is the viewport (in
// the format returned by OpenGL) as it was when the last event reached the
// tableau. For example, it is not possible to switch 'current' child without
// knowing the viewport, because a LEAVE/ENTER pair have to be sent to the old
// and new child and the viewport must be set correctly before dispatching these
// events.
//
// \author fsm

#include "vgui_poly_tableau.h"

#include <vcl_iostream.h>
#include <vcl_cassert.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_matrix_state.h>
#include <vgui/vgui_popup_params.h>

//-----------------------------------------------------------------------------
//: The constructor takes a snapshot of the current viewport and scissor areas.
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

//-----------------------------------------------------------------------------
vgui_poly_tableau::item::item(vgui_tableau* p, vgui_tableau_sptr const&c,
                              float x_, float y_, float w_, float h_,
                              int id_)
  : tab(p, c)
  , x(x_), y(y_)
  , w(w_), h(h_)
  , id(id_)
{
  outline_color[0] = outline_color[1] = outline_color[2] = 1; // outline in white
}

//-----------------------------------------------------------------------------
void vgui_poly_tableau::item::set_vp(GLint const vp[4])
{
  int region[4]={
    int(vp[0] + x*vp[2]), // x
    int(vp[1] + y*vp[3]), // y
    int(w*vp[2]),  // w
    int(h*vp[3])   // h
  };

  glViewport( region[0], region[1], region[2], region[3] );
  glScissor ( region[0], region[1], region[2], region[3] );
}

//-----------------------------------------------------------------------------
//: Returns true if the given position is inside the boundaries of this item.
bool vgui_poly_tableau::item::inside(GLint const vp[4],int vx, int vy) const
{
  float rx = float(vx-vp[0])/vp[2];
  float ry = float(vy-vp[1])/vp[3];

  bool ans = (x<=rx && rx<x+w) && (y<=ry && ry<y+h);
#ifdef DEBUG
  if (ans)
    vcl_cerr << "Point "<< vx << ' '<< vy <<" inside sub-window: "<< id << '\n';
#endif
  return ans;
}

//-----------------------------------------------------------------------------
//: Constructor - don't use this, use vgui_poly_tableu_new.
vgui_poly_tableau::vgui_poly_tableau()
  : vgui_tableau()
  , current(-1)
  , may_switch_child(true)
{
}

//-----------------------------------------------------------------------------
//: Destructor - called by vgui_poly_tableau_sptr.
vgui_poly_tableau::~vgui_poly_tableau()
{
}

//-----------------------------------------------------------------------------
//: Erase the item at the given position from the list of items.
void vgui_poly_tableau::erase(iterator i)
{
  assert(sub.begin()<=i && i<sub.end()); // wrong iterator for this container.

  if (current == i-sub.begin())
    current = -1;

  sub.erase(i);
}

//-----------------------------------------------------------------------------
//: Remove subtableau, referred to by handle.
void vgui_poly_tableau::remove(int id)
{
  for (iterator i=begin(); i!=end(); ++i)
    if (i->id == id) {
      erase(i);
      return;
    }
  vgui_macro_warning << "no such id " << id << vcl_endl;
}

//-----------------------------------------------------------------------------
//:  Move subtableau to a new location.
void vgui_poly_tableau::move(int id, float x, float y, float w, float h)
{
  for (iterator i=begin(); i!=end(); ++i)
    if (i->id == id) {
      i->x = x;
      i->y = y;
      i->w = w;
      i->h = h;
      post_redraw();
      return;
    }
  vgui_macro_warning << "no such id " << id << vcl_endl;
}

//-----------------------------------------------------------------------------
//: Replace the tableau with the given ID by the given tableau.
//  Keep the same ID and do not change the value of 'current'.
void vgui_poly_tableau::replace(int id, vgui_tableau_sptr const& tab)
{
  for (iterator i=begin(); i!=end(); ++i)
    if (i->id == id) {
      i->tab.assign(tab);
      if (tab)
        tab->post_redraw();
      else
        post_redraw();
      return;
    }
  vgui_macro_warning << "no such id " << id << vcl_endl;
}

//-----------------------------------------------------------------------------
//: Returns the tableau with the given ID.
vgui_tableau_sptr vgui_poly_tableau::get(int id) const
{
  for (const_iterator i=begin(); i!=end(); ++i)
    if (i->id == id)
      return i->tab;
  return vgui_tableau_sptr();
}

//-----------------------------------------------------------------------------
//: Sets the color that the tableau is outlined with .
void vgui_poly_tableau::set_outline_color(const int id, const int r,
                                          const int g, const int b)
{
  for (unsigned i=0; i<sub.size(); ++i) {
    if (sub[i].id == id) {
      sub[i].outline_color[0] = r;
      sub[i].outline_color[1] = g;
      sub[i].outline_color[2] = b;
    }
  }
}

//-----------------------------------------------------------------------------
//: Adds the given tableau to the given proportion of the viewport.
//  x,y,w,h specify a portion of the vgui_poly_tableau's viewport in coordinates
//  which go from 0 to 1.
int vgui_poly_tableau::add(vgui_tableau_sptr const& t, float x, float y,
                           float w, float h)
{
  static int counter = 0;
  assert(counter < 1000000); // a million. FIXME.
  item it(this, t, x, y, w, h, ++counter) ;
  sub.push_back(it);
#ifdef DEBUG
  vcl_cerr << "id = " << sub.back().id << '\n'
           << "x  = " << sub.back().x << '\n'
           << "y  = " << sub.back().y << '\n'
           << "w  = " << sub.back().w << '\n'
           << "h  = " << sub.back().h << '\n';
#endif
  return counter;
}

//-----------------------------------------------------------------------------
//: Misnomer - gets index of the child currently under the pointer's position.
int vgui_poly_tableau::get_active(GLint const vp[4], int wx, int wy) const
{
  int act = -1;
  for (unsigned i=0; i<sub.size(); ++i)
    if (sub[i].inside(vp, wx, wy) )
      act = i;
  return act;
}

//-----------------------------------------------------------------------------
//: Returns the ID of the current child.
int vgui_poly_tableau::get_current_id()
{
  return (current != -1) ? sub[current].id : -1;
}

//-----------------------------------------------------------------------------
//: Sets the child under the pointer to current.
void vgui_poly_tableau::set_current(GLint const vp[4], int index)
{
  if (current == index)
    return;

  // send leave event to old current subtableau :
  if (current != -1) {
    vgui_event e(vgui_LEAVE);
    sub[current].set_vp(vp);
    sub[current].tab->handle(e);
  }

  // switch :
#ifdef DEBUG
  vcl_cerr << "vgui_poly_tableau::set_current: switch from " << current << " to " << index << '\n';
#endif
  current = index;

  // send enter event to new current subtableau :
  if (current != -1) {
    vgui_event e(vgui_ENTER);
    sub[current].set_vp(vp);
    sub[current].tab->handle(e);
  }
}

//-----------------------------------------------------------------------------
//: Handles events for this tableau and passes unused ones to the correct child.
bool vgui_poly_tableau::handle(GLint const vp[4], vgui_event const &e)
{
  // Draw events must go to all children, in the right order.
  if (e.type==vgui_DRAW || e.type==vgui_DRAW_OVERLAY) {
    // save current matrix state so that we can restore it
    // after drawing each item (so that the next item will
    // have correct settings).
    vgui_matrix_state PM(false);
    PM.save();

    for (unsigned i=0; i<sub.size(); ++i) {
      PM.restore();

      // set viewport for child.
      sub[i].set_vp(vp);

      // let child handle the event.
      sub[i].tab && sub[i].tab->handle(e);

      if (e.type == vgui_DRAW) {
        // draw border of child.
        vgui_matrix_state::identity_gl_matrices();
        glColor3f(sub[i].outline_color[0],
                  sub[i].outline_color[1],
                  sub[i].outline_color[2]);
        glLineWidth(1);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, vp[2], 0, vp[3], -1, +1);
        glBegin(GL_LINE_LOOP);
        glVertex2d(      0, 0);
        glVertex2d(vp[2]-1, 0);
        glVertex2d(vp[2]-1, vp[3]-1);
        glVertex2d(      0, vp[3]-1);
        glEnd();
      }
    }

    return true;
  }

  // Timer events go to each child, till handled.
  if (e.type == vgui_TIMER) {
    vgui_matrix_state PM(false);
    PM.save();
    bool handled = false;
    for (unsigned i=0; i<sub.size(); ++i) {
      PM.restore();
      sub[i].set_vp(vp);
      if (sub[i].tab && sub[i].tab->handle(e)) {
        handled = true;
        break;
      }
    }
    return handled;
  }

  // All other events (including leave and enter) are passed
  // to the current child, if there is one.
  if (current != -1) {
    sub[current].set_vp(vp);
    return sub[current].tab && sub[current].tab->handle(e);
  }

  // In case of doubt, or if there is no current child, return false.
  return false;
}

//-----------------------------------------------------------------------------
//: Returns the type of this tableau ('vgui_poly_tableau').
vcl_string vgui_poly_tableau::type_name() const
{
  return "vgui_poly_tableau";
}

//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool vgui_poly_tableau::handle(vgui_event const &e)
{
  // Take snapshot of the viewport and scissor areas
  vgui_poly_tableau_vp_sc_snapshot snap;
  glEnable(GL_SCISSOR_TEST);

  // pointer motion
  if (e.type == vgui_MOTION)
  {
    // switch child, if necessary
    if (may_switch_child) {
      int active = get_active(snap.vp, e.wx, e.wy);
      if (active!=-1 && active!=get_current())
        set_current(snap.vp, active);
    }
    return handle(snap.vp, e);
  }

  // button down
  else if (e.type == vgui_BUTTON_DOWN)
  {
    // disallow child switch
    may_switch_child = false;

    return handle(snap.vp, e);
  }

  // button up
  else if (e.type == vgui_BUTTON_UP)
  {
    // Call this first because the button might be released over a
    // child other than the current one.
    bool f = handle(snap.vp, e);

    // allow child switch. (assumes all buttons are released now).
    may_switch_child = true;

    // switch child, if necessary
    int active = get_active(snap.vp, e.wx, e.wy);
    if (active!=-1 && active!=get_current())
      set_current(snap.vp, active);

     return f;
  }
  return handle(snap.vp, e);
}

//-----------------------------------------------------------------------------
void vgui_poly_tableau::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  if (params.recurse) {
    int index = get_current();
    if (index >=0)
      sub[index].tab->get_popup(params, menu);
  }
}
