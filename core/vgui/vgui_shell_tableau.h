// This is core/vgui/vgui_shell_tableau.h
#ifndef vgui_shell_tableau_h_
#define vgui_shell_tableau_h_
//:
// \file
// \author fsm
// \brief  Tableau to go at the top of one's tableau hierarchy.
//
//  Contains classes:  vgui_shell_tableau  vgui_shell_tableau_new  vgui_shell_tableau_bindings
//
// \verbatim
//  Modifications
//   18 Sept 00 capes@robots. Added set_enable_key_bindings().
//                            Key bindings are OFF by default.
//   07-Aug-2002 K.Y.McGaul - Converted to Doxygen style comments.
// \endverbatim

#include "vgui_shell_tableau_sptr.h"

#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_clear_tableau_sptr.h>
#include <vgui/vgui_tview_launcher_tableau_sptr.h>
#include <vgui/vgui_event_condition.h>


//: Key bindings for vgui_shell_tableau
//
// The events are
//
// - quit (Alt-Q), which exits the application
//
// - close (Alt-W), which closes the window (and exits the application
//   if there is only one window)
//
// The set_default_* member functions can be used to change the
// default bindings for all *future* shell tableaux. It will not
// change the bindings of already created tableaux.
//
class vgui_shell_tableau_bindings
{
 public:
  //:
  vgui_shell_tableau_bindings()
    : quit( default_quit ), close( default_close ), graph( default_graph ) {}

  //:
  vgui_shell_tableau_bindings& set_quit( vgui_event_condition cond )
  { quit = cond; return *this; }

  //:
  vgui_shell_tableau_bindings& set_close( vgui_event_condition cond )
  { close = cond; return *this; }

  vgui_shell_tableau_bindings& set_graph( vgui_event_condition cond )
  { graph = cond; return *this; }

  //:
  static void set_default_quit( vgui_event_condition cond )
  { default_quit = cond; }

  //:
  static void set_default_close( vgui_event_condition cond )
  { default_close = cond; }

  //:
  static void set_default_graph( vgui_event_condition cond )
  { default_graph = cond; }

  vgui_event_condition quit;
  vgui_event_condition close;
  vgui_event_condition graph;

  static vgui_event_condition default_quit;
  static vgui_event_condition default_close;
  static vgui_event_condition default_graph;
};


//:  Tableau to go at the top of one's tableau hierarchy.
//
// A shell tableau is a handy collection of things one often wants
// at the very top of one's tableau hierarchy. It is essentially an
// acetate with m utility tableaux at the bottom :
//
// \verbatim
//    m+n your_tableau_n <--- last user added child.
//    .   ...
//    .   ...
//    m=3 your_tableau_0 <--- first user added child.
// ^  2   clear_tableau
// |  1   quit_tableau
// |  0   tview_launch_tableau <--- first child.
// \endverbatim
class vgui_shell_tableau : public vgui_composite_tableau
{
  typedef vgui_shell_tableau_bindings key_bindings_type;

  bool do_quit;
  bool enable_key_bindings;
  key_bindings_type bindings;
  vgui_clear_tableau_sptr clear;
  vgui_tview_launcher_tableau_sptr graph;
 public:
  vgui_shell_tableau();
  vgui_shell_tableau(vgui_tableau_sptr const &);
  vgui_shell_tableau(vgui_tableau_sptr const &,
                     vgui_tableau_sptr const &);
  vgui_shell_tableau(vgui_tableau_sptr const &,
                     vgui_tableau_sptr const &,
                     vgui_tableau_sptr const &);
  vgui_shell_tableau(vgui_tableau_sptr const &,
                     vgui_tableau_sptr const &,
                     vgui_tableau_sptr const &,
                     vgui_tableau_sptr const &);

  bool handle(vgui_event const &);
  vcl_string type_name() const;

  void get_popup(vgui_popup_params const &, vgui_menu &);
  void set_quit(bool on);
  void set_enable_key_bindings(bool on);

  vgui_clear_tableau_sptr get_clear() const { return clear; }
  vgui_tview_launcher_tableau_sptr get_graph() const { return graph; }

 protected:
  ~vgui_shell_tableau();

 private:
  void init();
};

//: Create a smart-pointer to a vgui_shell_tableau.
struct vgui_shell_tableau_new : public vgui_shell_tableau_sptr
{
  typedef vgui_shell_tableau_sptr base;
  vgui_shell_tableau_new()
    : base(new vgui_shell_tableau()) { }
  vgui_shell_tableau_new(vgui_tableau_sptr const &a)
    : base(new vgui_shell_tableau(a)) { }
  vgui_shell_tableau_new(vgui_tableau_sptr const &a,vgui_tableau_sptr const &b)
    : base(new vgui_shell_tableau(a, b)) { }
  vgui_shell_tableau_new(vgui_tableau_sptr const &a,vgui_tableau_sptr const &b,vgui_tableau_sptr const &c)
    : base(new vgui_shell_tableau(a, b, c)) { }
  vgui_shell_tableau_new(vgui_tableau_sptr const &a,vgui_tableau_sptr const &b,vgui_tableau_sptr const &c,vgui_tableau_sptr const&d)
    : base(new vgui_shell_tableau(a, b, c, d)) { }
};

#endif // vgui_shell_tableau_h_
