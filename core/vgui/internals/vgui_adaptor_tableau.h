#ifndef vgui_adaptor_tableau_h_
#define vgui_adaptor_tableau_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_adaptor_tableau
// .INCLUDE vgui/internals/vgui_adaptor_tableau.h
// .FILE internals/vgui_adaptor_tableau.cxx
// @author fsm@robots.ox.ac.uk

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_slot.h>
class vgui_adaptor;

class vgui_adaptor_tableau : private vgui_tableau
{
  friend class vgui_adaptor;
  
  vgui_adaptor_tableau(vgui_adaptor *a);
  ~vgui_adaptor_tableau();
  
  void set_child(vgui_tableau_ref const & t) { slot.assign(t); }
  vgui_tableau_ref get_child() const { return slot; }
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
