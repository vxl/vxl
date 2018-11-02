// Example of creation/deletion of multiple windows in an application.
// The reference counted tableaux take care of memory deallocation for you.
// Which is nice.
// capes@robots.ox.ac.uk Sept 2000

// You can close views by 3 methods :
// 1 - clicking the 'X'
// 2 - selecting the "Close View" menu option
// 3 - pressing Meta-W (caught by vgui_quit_tableau and vgui_shell_tableau)

#include <cstdlib>
#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_load.h>
#include <vgui/vgui.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_text_graph.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_tableau.h>

int window_count = 0;

// Global viewer2D tableau (wrapping an image_tableau) which
// will appear in every window.
vgui_viewer2D_tableau_sptr global_viewer_tab;

// Make a test tableau which catches the vgui_DESTROY event.

class example_flim_tableau : public vgui_tableau {
  bool handle(vgui_event const& e) {
    if (e.type == vgui_DESTROY) {
      vgui_dialog dialog("Farewell!");
      dialog.message("I am about to die!\nLast chance to save data would go here...");
      dialog.set_modal(true);
      dialog.ask();
      return true;
    }
    else
      return false;
  }
  std::string type_name() const { return "example_flim_tableau"; }
};

typedef vgui_tableau_sptr_t<example_flim_tableau> example_flim_tableau_sptr;

struct example_flim_tableau_new : public example_flim_tableau_sptr {
  typedef example_flim_tableau_sptr base;
  example_flim_tableau_new() : base(new example_flim_tableau()) { }
};

// "Close View" menu callback
static void hedphelym(const void* const_data)
{
  // a static_cast<> may not cast away const.
  void *data = const_cast<void *>(const_data);
  vgui_adaptor* adaptor = static_cast<vgui_adaptor*>(data);
  adaptor->post_destroy();
}

// "New View" menu callback
static void ptolemy()
{
  vgui_text_tableau_new text_tab;
  std::stringstream s; s << "This is view " << window_count;
  text_tab->add(256,256,s.str());

  example_flim_tableau_new flim_tab;

  vgui_shell_tableau_new shell_tab(global_viewer_tab, text_tab, flim_tab);
  shell_tab->set_enable_key_bindings(true);

  vgui_window* sub_window = vgui::produce_window(512, 512);
  sub_window->get_adaptor()->set_tableau(shell_tab);

  vgui_menu menu;
  // Pass a pointer to the adaptor as data to the menu callback so that
  // it knows which adaptor to destroy.
  menu.add("Close view", hedphelym, sub_window->get_adaptor());
  sub_window->set_menubar(menu);

  sub_window->show();
  s.clear(); s << "Window " << window_count++;
  sub_window->get_statusbar()->write(s.str().c_str());
}

// -----------------------------------------------------------------------------
// Now do some stuff...
int main (int argc, char** argv) {

  //vgui::select("gtk");   // Multiple windows only implemented for gtk adaptor at present!
  vgui::init(argc, argv);

  if (argc < 2) {
    std::cerr << __FILE__ " : image_file argument required\n";
    std::abort();
  }

  vil1_image img = vil1_load(argv[1]);

  vgui_image_tableau_new img_tab(img);
  global_viewer_tab = vgui_viewer2D_tableau_new(img_tab);

  int wd=img.width(), ht=img.height();
  vgui_text_tableau_new text_tab;
  std::stringstream s; s << "This is view " << window_count;
  text_tab->add(wd*.5f-100,ht*.5f,s.str());

  example_flim_tableau_new flim_tab;

  vgui_shell_tableau_new shell_tab(global_viewer_tab, text_tab, flim_tab);
  shell_tab->set_enable_key_bindings(true);

  vgui_menu menu;
  menu.add("Add new view", ptolemy);

  vgui_window* main_window = vgui::produce_window(wd, ht, menu);
  main_window->get_adaptor()->set_tableau(shell_tab);
  main_window->show();

  s.clear(); s << "Window " << window_count++;
  main_window->get_statusbar()->write(s.str().c_str());

  vgui_text_graph(std::cerr);

  return vgui::run();
}
