// This is core/vgui/internals/vgui_adaptor_tableau.h
#ifndef vgui_adaptor_tableau_h_
#define vgui_adaptor_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \brief  Propagates tableau posts to an adaptor.

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
class vgui_adaptor;

//: Propagates tableau posts to an adaptor.
// This is a special tableau which is always the top
// of the tableau graph under each adaptor.  (Top means
// it has no parents and every tableau under the adaptor
// is a child of one of these).  Its main purpose
// is to catch messages "up" the tableau hierarchy
// like "post_redraw", and pass them on to the
// adaptor.
class vgui_adaptor_tableau : private vgui_tableau
{
  friend class vgui_adaptor;

  vgui_adaptor_tableau(vgui_adaptor *a);
  ~vgui_adaptor_tableau();

  void set_child(vgui_tableau_sptr const& t) { slot.assign(t); }
  vgui_parent_child_link get_child() const { return slot; }
  bool handle(vgui_event const &e) { return slot.handle(e); }
  vcl_string type_name() const { return "vgui_adaptor_tableau"; }

//even more private:
  vgui_adaptor *adaptor;
  vgui_parent_child_link slot;
  void post_message(char const *, void const *);
  void post_redraw();
  void post_overlay_redraw();
  void post_idle_request();
};

#endif // vgui_adaptor_tableau_h_
