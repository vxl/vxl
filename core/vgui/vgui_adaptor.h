// This is oxl/vgui/vgui_adaptor.h
#ifndef vgui_adaptor_h_
#define vgui_adaptor_h_
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  Provides an abstract interface to an OpenGL rendering state.
//         Contains classes: vgui_adaptor.
//
// \verbatim
// Modifications
// 20 October  1999 pcp@robots. removed derivation from tableau.
// 20 Octover  1999 fsm@robots. detach()ed the adaptor from the tableau in
//                  the destuctor to avoid leaving a dangling pointer in the
//                  tableau.
// 28 October  1999 fsm@robots. added more menu support (as agreed with pcp).
//  7 January  1999 fsm@robots. new observer/observable method of getting
//                  redraw from tableau to adaptor.
//  2 February 1999 fsm@robots. more overlay documentation.
// 23-AUG-2000      Marko Bacic,Oxford RRG - Added post_message
// 31 August   2000 u97mb@robots. Support for mfc acceleration
// 07-SEP-2000      Marko Bacic,Oxford RRG - Added pure virtual method "draw"
// 20 Sept     2000 capes@robots. added post_destroy() method.
// Feb.2002         Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include "dll.h"
#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
class vgui_window;
class vgui_tableau;
struct vgui_tableau_sptr;
class vgui_popup_params;
class vgui_adaptor_tableau;

//: Provides an abstract interface to an OpenGL rendering state.
//
// Provides an abstract interface to an OpenGL rendering state (context)
// producing events. Events from adaptor (eg. when someone presses a
// mouse button over the window) will be dispatched to the associated
// vgui_tableau in a vgui_event structure. Draw requests will be dispatched
// to the tableau as vgui_DRAW and vgui_DRAW_OVERLAY events.
//
// Note. When the set_tableau() method is called, the adaptor will attach itself
// to the tableau as an observer. The tableau may detach that observer if it so
// chooses, but that would be a bad idea as it would stop post()s from tableaux
// reaching the adaptor.

class vgui_adaptor {
public:
  vgui_adaptor();
  virtual ~vgui_adaptor();

  // ---------- mixin methods ----------

  // tableau related.
  vgui_tableau_sptr get_tableau() const;
  void set_tableau(vgui_tableau_sptr const& t);

  // popup related.
  vgui_menu get_total_popup(vgui_popup_params &) const;
  void config_dialog();
  // why not just make 'menu' a public member?
  vgui_menu &get_popup() { return menu; }
  void set_popup(vgui_menu const &m) { get_popup() = m; }
  void include_in_popup(vgui_menu const &m) { get_popup().include(m); }

  // event related.
  bool dispatch_to_tableau(vgui_event const &);

  //: This static datum points to the adaptor that last received a mouse event.
  //  It is set to zero if its pointee receives a LEAVE event.  It will
  // often point to the adaptor which was current when a popup menu was
  // triggered, which is often what one wants it for.
  static vgui_DLLDATA vgui_adaptor *current;

  // ---------- virtuals ----------
  // only the pure virtuals need be implemented.
  // the rest is quality-of-implementation stuff.

  //: Return width of rendering area.
  // *Not* the width of the viewport. There seems to be no OpenGL 
  // mechanism for doing this.
  virtual unsigned get_width() const =0;
  //: Return height of rendering area.
  // *Not* the height of the viewport. There seems to be no OpenGL 
  // mechanism for doing this.
  virtual unsigned get_height() const =0;

  // These methods are called by vgui_adaptor (in its capacity as a base class)
  // when a post arrives.
  virtual int  post_timer(float);
  virtual void post_timer(float, int );
  //: Redraw the rendering area.
  virtual void post_redraw() =0;
  virtual void post_overlay_redraw() =0;
  virtual void post_message(char const *, void const *);
  virtual void post_destroy();

  // popup menus
  virtual void bind_popups(vgui_modifier =vgui_MODIFIER_NULL, vgui_button =vgui_RIGHT);
  virtual void get_popup_bindings(vgui_modifier &, vgui_button &) const;

  // getting the window.
  virtual vgui_window *get_window() const;

  // various buffer behaviour.
  virtual void swap_buffers();
  virtual void make_current();

protected:
  //: Parameters controlling the popup menu style.
  // These are put onto a popup_params object and passed to tableau::get_popup()
  bool nested_popups;
  bool default_items;

private:
  vgui_adaptor_tableau *the_tableau;

  //: this menu is put before the tableau's popup menu.
  vgui_menu menu;
};

#endif // vgui_adaptor_h_
