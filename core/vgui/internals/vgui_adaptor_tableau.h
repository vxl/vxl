// This is oxl/vgui/internals/vgui_adaptor_tableau.h
#ifndef vgui_adaptor_tableau_h_
#define vgui_adaptor_tableau_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  Propagates tableau posts to an adaptor.

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
class vgui_adaptor;

//: Propagates tableau posts to an adaptor.
class vgui_adaptor_tableau : private vgui_tableau
{
  friend class vgui_adaptor;

  vgui_adaptor_tableau(vgui_adaptor *a);
  ~vgui_adaptor_tableau();

  void set_child(vgui_tableau_sptr const & t) { slot.assign(t); }
  vgui_tableau_sptr get_child() const { return slot; }
  bool handle(vgui_event const &e) { return slot.handle(e); }
  vcl_string type_name() const { return "vgui_adaptor_tableau"; }

/*even more*/ private:
  vgui_adaptor *adaptor;
  vgui_slot slot;
  void post_message(char const *, void const *);
  void post_redraw();
  void post_overlay_redraw();
};

#endif // vgui_adaptor_tableau_h_
