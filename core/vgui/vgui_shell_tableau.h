#ifndef vgui_shell_tableau_h_
#define vgui_shell_tableau_h_
// .NAME vgui_shell_tableau
// .INCLUDE vgui/vgui_shell_tableau.h
// .FILE vgui_shell_tableau.cxx
//
// .SECTION Description
// A shell tableau is a handy collection of things one often wants 
// at the very top of one's tableau hiearchy. It is essentially an
// acetate with m utility tableaux at the bottom :
// 
//    m+n your_tableau_n <--- last user added child.
//    .   ...
//    .   ...
//    m=3 your_tableau_0 <--- first user added child.
// ^  2   clear_tableau
// |  1   quit_tableau
// |  0   tview_launcher <--- first child.
// @author fsm@robots.ox.ac.uk
//
// .SECTION Modifications
// 18 Sept 00 capes@robots. Added set_enable_key_bindings(). Key bindings are OFF by default.

#include "vgui_shell_tableau_ref.h"
#include <vgui/vgui_composite.h>
#include <vgui/vgui_clear_tableau_ref.h>
#include <vgui/vgui_tview_launcher_ref.h>
//class vgui_quit_tableau;


class vgui_shell_tableau : public vgui_composite {
public:
  vgui_shell_tableau();
  vgui_shell_tableau(vgui_tableau_ref const &);
  vgui_shell_tableau(vgui_tableau_ref const &, vgui_tableau_ref const &);
  vgui_shell_tableau(vgui_tableau_ref const &, vgui_tableau_ref const &,vgui_tableau_ref const &);
  vgui_shell_tableau(vgui_tableau_ref const &, vgui_tableau_ref const &,vgui_tableau_ref const &,vgui_tableau_ref const &);
  
  bool handle(vgui_event const &);
  vcl_string type_name() const;

  void get_popup(vgui_popup_params const &, vgui_menu &);
  void set_quit(bool on) { do_quit = on; }
  void set_enable_key_bindings(bool on) { enable_key_bindings = on; }

  vgui_clear_tableau_ref get_clear() const { return clear; }
  vgui_tview_launcher_ref get_graph() const { return graph; }

protected:
  ~vgui_shell_tableau();
  
private:
  void init();

  bool do_quit;
  bool enable_key_bindings;
  vgui_clear_tableau_ref clear;
  vgui_tview_launcher_ref graph;
};

struct vgui_shell_tableau_new : public vgui_shell_tableau_ref {
  typedef vgui_shell_tableau_ref base;
  vgui_shell_tableau_new() : base(new vgui_shell_tableau()) { }
  vgui_shell_tableau_new(vgui_tableau_ref const &a) : base(new vgui_shell_tableau(a)) { }
  vgui_shell_tableau_new(vgui_tableau_ref const &a, vgui_tableau_ref const &b) : base(new vgui_shell_tableau(a, b)) { }
  vgui_shell_tableau_new(vgui_tableau_ref const &a, vgui_tableau_ref const &b,vgui_tableau_ref const &c) : base(new vgui_shell_tableau(a, b, c)) { }
  vgui_shell_tableau_new(vgui_tableau_ref const &a, vgui_tableau_ref const &b,vgui_tableau_ref const &c,vgui_tableau_ref const &d) : base(new vgui_shell_tableau(a, b, c, d)) { }
};

#endif // vgui_shell_tableau_h_
