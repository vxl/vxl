// This is oxl/ogui/examples/fsm/display-images.cxx

//:
// \file
// This example displays some images in a window.

#include <vul/vul_file.h>

#include <vgui/vgui.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_shell_tableau.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_macro.h>

struct example_window_title_setter : vgui_observer
{
  char **argv;
  vgui_deck_tableau_sptr deck;

  example_window_title_setter(char **argv_, vgui_deck_tableau_sptr const &deck_)
    : argv(argv_)
    , deck(deck_)
  {
    deck->observers.attach(this);
  }

  ~example_window_title_setter()
  {
    deck->observers.detach(this);
  }

  vcl_string last_title;

  void update()
  {
    vgui_adaptor *a = vgui_adaptor::current;
    if (! a) {
      vgui_macro_warning << "no adaptor\n";
      return;
    }
    vgui_window *w = a->get_window();
    if (! w) {
      vgui_macro_warning << "no window\n";
      return;
    }
    int i = deck->index();
    if (last_title != argv[i + 1]) {
      last_title = argv[i + 1];
      w->set_title(last_title);
    }
  }
};

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  vgui_deck_tableau_new deck;
  unsigned int w = 0, h = 0;
  for (int i=1; i<argc; ++i) {
    if (vul_file::exists(argv[i])) {
      vgui_image_tableau_new t(argv[i]);
      if (t->width ()>w) w = t->width ();
      if (t->height()>h) h = t->height();
      deck->add(t);
    }
    else
      vcl_cerr << "no such file : \'" << argv[i] << "\'\n";
  }
  deck->index(0);

  // make an observer of the deck. it will set the window title
  // every time the deck's observers are notified.
  example_window_title_setter wawa(argv, deck);

  vgui_viewer2D_new viewer(deck);

  vgui_shell_tableau_new shell(viewer);
  shell->set_enable_key_bindings(true);

  return vgui::run(shell, w, h);
}
