//:
// \file
// \brief  Example of text input/output windows.
// \author Peter Vanroose
// \date   March 2003

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vgui/vgui.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_statusbar.h>
#include <vgui/vgui_text_graph.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_tableau.h>

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
  vcl_string type_name() const { return "example_flim_tableau"; }
};

typedef vgui_tableau_sptr_t<example_flim_tableau> example_flim_tableau_sptr;

struct example_flim_tableau_new : public example_flim_tableau_sptr {
  typedef example_flim_tableau_sptr base;
  example_flim_tableau_new() : base(new example_flim_tableau()) { }
};

// -----------------------------------------------------------------------------
// Now do some stuff...
int main (int argc, char** argv)
{
  vgui::init(argc, argv);

  if (argc < 2) {
    vcl_cerr << __FILE__ " : image_file argument required\n";
    vcl_abort();
  }

  vil_image_view<vxl_byte> img = vil_load(argv[1]);
  if (!img) {
    vcl_cerr << __FILE__ " : cannot load image from " << argv[1] << '\n';
    vcl_abort();
  }

  vgui_image_tableau_new img_tab(img);

  int wd=img.ni(), ht=img.nj();
  vgui_text_tableau_new text_tab;
  text_tab->add(wd,ht,"text 2");

  example_flim_tableau_new flim_tab;

  vgui_shell_tableau_new shell_tab(img_tab, text_tab, flim_tab);
  shell_tab->set_enable_key_bindings(true);

  vgui_window* main_window = vgui::produce_window(wd, ht);
  main_window->get_adaptor()->set_tableau(shell_tab);
  main_window->show();

  main_window->get_statusbar()->write("statusbar-text");

  vgui_text_graph(vcl_cerr);

  return vgui::run();
}
