// This is core/vgui/vgui_adaptor.h
#ifndef vgui_adaptor_h_
#define vgui_adaptor_h_
//:
// \file
// \author fsm
// \brief  Provides an abstract interface to an OpenGL rendering state.
//
// \verbatim
// Modifications
// 20 October  1999 pcp@robots. removed derivation from tableau.
// 20 October  1999 fsm. detach()ed the adaptor from the tableau in
//                  the destructor to avoid leaving a dangling pointer in the
//                  tableau.
// 28 October  1999 fsm. added more menu support (as agreed with pcp).
//  7 January  1999 fsm. new observer/observable method of getting
//                  redraw from tableau to adaptor.
//  2 February 1999 fsm. more overlay documentation.
// 23-AUG-2000      Marko Bacic,Oxford RRG - Added post_message
// 31 August   2000 u97mb@robots. Support for mfc acceleration
// 07-SEP-2000      Marko Bacic,Oxford RRG - Added pure virtual method "draw"
// 20 Sept     2000 capes@robots. added post_destroy() method.
// Feb.2002         Peter Vanroose - brief doxygen comment placed on single line
// 08-OCT-2002 K.Y.McGaul - Added use_double_buffering.
// 23-DEC-2002 J.L.Mundy - Added access for the window
// 23-DEC-2002 A.Perera - Removed access for the window. It doesn't belong at this level.
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
class vgui_adaptor
{
 public:
  //: Constructor - create a default adaptor.
  vgui_adaptor();

  //: Destructor - quits application if all adaptors have been deleted.
  virtual ~vgui_adaptor();

  //: Get the vgui_tableau associated with this vgui_adaptor.
  vgui_tableau_sptr get_tableau() const;

  //: Set the vgui_tableau associated with this vgui_adaptor.
  void set_tableau(vgui_tableau_sptr const& t);

  //: True to use double buffering, false to not use it.
  void set_double_buffering(bool b) { use_double_buffering = b; }

  //: Return the concatenation of the adaptor's menu with the tableau's menu.
  vgui_menu get_total_popup(vgui_popup_params &) const;

  //: Pop up a dialog for changing the popup modifier and button bindings.
  void config_dialog();

  //: Return the popup menu for this adaptor.
  vgui_menu &get_popup() { return menu; }

  //: Set the popup menu for this adaptor.
  void set_popup(vgui_menu const &m) { get_popup() = m; }

  //: Add the given menu to the popup menu for this adaptor.
  void include_in_popup(vgui_menu const &m) { get_popup().include(m); }

  //: Dispatches the given event to the tableau.
  //  This method performs various checks which can be performed generically for
  //  all tableaux. It is not the responsibility of this method to take care of
  //  overlay handling - the derived class must sort that out.
  bool dispatch_to_tableau(vgui_event const &);

  //: This static datum points to the adaptor that last received a mouse event.
  //  It is set to zero if its pointer receives a LEAVE event.  It will
  // often point to the adaptor which was current when a popup menu was
  // triggered, which is often what one wants it for.
  static vgui_DLLDATA vgui_adaptor *current;

  // ---------- virtuals ----------
  // only the pure virtuals need be implemented.
  // the rest is quality-of-implementation stuff.

  //: Return width of rendering area.
  //  *Not* the width of the viewport. There seems to be no OpenGL
  //  mechanism for doing this.
  virtual unsigned get_width() const =0;

  //: Return height of rendering area.
  //  *Not* the height of the viewport. There seems to be no OpenGL
  //  mechanism for doing this.
  virtual unsigned get_height() const =0;

  // These methods are called by vgui_adaptor (in its capacity as a base class)
  // when a post arrives.
  // The single-argument version calls the two-argument one with increasing
  // counter value on subsequent calls.  The used counter value is returned.
  // The two-argument version must be implemented by the derived class.
  virtual void post_timer(float, int );
          int  post_timer(float);
  //: Redraw the rendering area.
  virtual void post_redraw() =0;
  virtual void post_overlay_redraw() =0;
  virtual void post_message(char const *, void const *);
  virtual void post_destroy();

  //: Bind the given modifier/button combination to the popup menu.
  virtual void bind_popups(vgui_modifier=vgui_MODIFIER_NULL, vgui_button=vgui_RIGHT);

  //: Return the modifier/button which pops up the popup menu.
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

  //: Whether or not to use double buffering.
  bool use_double_buffering;

 private:
  vgui_adaptor_tableau *the_tableau;

  //: this menu is put before the tableau's popup menu.
  vgui_menu menu;
};

#endif // vgui_adaptor_h_
