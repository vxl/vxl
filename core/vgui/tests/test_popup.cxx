/*
  fsm
*/
#include <cstdlib>
#include <iostream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "vgui/vgui.h"
#include "vgui/vgui_window.h"
#include "vgui/vgui_adaptor.h"
#include "vgui/vgui_menu.h"
#include "vgui/vgui_tableau.h"
#include "vgui/vgui_command.h"

// example of a tableau which has an associated menu :
struct joe_tab : public vgui_tableau
{
  std::string msg;
  vgui_menu   the_menu;

  joe_tab(const std::string & m)
    : msg(m)
  {
    the_menu.add("joe(0)", joe_callback, nullptr /*data*/);
    the_menu.add("joe(this)", joe_callback, this /*data*/);
    the_menu.add("toggle1", new vgui_command_toggle(false));
    the_menu.add("toggle2", new vgui_command_toggle(true));
  }
  ~joe_tab() {}

  static void
  joe_callback(void const * data)
  {
    std::cerr << "this is joe_tab::joe_callback(" << data << ")\n";
    if (data)
      std::cerr << "  msg=\"" << static_cast<const joe_tab *>(data)->msg << "\"\n";
  }
  void
  add_popup(vgui_menu & m)
  {
    m.include(the_menu);
  }
  bool
  handle(const vgui_event &)
  {
    return true;
  }
  std::string
  type_name() const
  {
    return "joe_tab";
  }
};

//--------------------------------------------------------------------------------

static void
SEGV()
{
  ++*(int *)nullptr;
}

static void
BUS()
{
  double x = 2.718281828;
  x = *(double *)(1 + (char *)&x);
}

static void
FPE()
{
  int one = 1;
  int zero = 0;
  int bad = one / zero;
  (void)bad; // quell compiler warning.
}

static void
ABRT()
{
  std::abort();
}

vgui_menu
get_fault_menu()
{
  vgui_menu menu;
  menu.add("SEGV", SEGV);
  menu.add("BUS", BUS);
  menu.add("FPE", FPE);
  menu.add("ABRT", ABRT);
  return menu;
}

//--------------------------------------------------------------------------------

static void
Quit()
{
  std::exit(0);
}

static void
Load()
{
  std::cerr << "Load\n";
}

static void
Save()
{
  std::cerr << "Load\n";
}

vgui_menu
get_file_menu()
{
  vgui_menu menu;
  menu.add("Quit", Quit);
  menu.add("LoadImage", Load, vgui_key('l'));
  menu.add("SaveImage", Save, vgui_key('s'));
  return menu;
}

//--------------------------------------------------------------------------------

int
main(int argc, char ** argv)
{
  vgui::init(argc, argv);

  // window 1
  vgui_window * win1 = vgui::adapt(new joe_tab("Joe1"), 256, 256, get_fault_menu(), "Joe1");
  win1->get_adaptor()->bind_popups(vgui_CTRL, vgui_RIGHT);
  win1->show();
  std::cerr << "Joe1 : ctrl+right for popup\n";

  // window 2
  vgui_window * win2 = vgui::adapt(new joe_tab("Joe2"), 256, 256, get_file_menu(), "Joe2");
  win2->get_adaptor()->bind_popups(vgui_SHIFT, vgui_LEFT);
  win2->show();
  std::cerr << "Joe2 : shift+left for popup\n";

  return vgui::run();
}
