// This is core/vgui/examples/example_vgui_observer.cxx
// This example displays some images in a window (the image filenames are
// given as parameters on the command line).  An observer is attached to the
// vgui_deck_tableau containing the images.  As the user flips through the
// images (using PageUp and PageDown), the observer changes the window
// title to the name of the appropriate image file.

#include <vul/vul_file.h>

#include <vgui/vgui.h>
#include <vgui/vgui_deck_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_shell_tableau.h>

#include <vgui/vgui_observer.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_macro.h>

struct example_window_title_setter : public vgui_observer
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

  // When this observer receives an update message, it changes the window title.
  void update()
  {
    vgui_adaptor *a = vgui_adaptor::current;
    if (! a)
    {
      vgui_macro_warning << "no adaptor\n";
      return;
    }
    vgui_window *w = a->get_window();
    if (! w)
    {
      vgui_macro_warning << "no window\n";
      return;
    }
    int i = deck->index();
    if (last_title != argv[i + 1])
    {
      last_title = argv[i + 1];
      w->set_title(last_title);
    }
  }
  void update(vgui_message const&) { update(); } // just ignore the message
  void update(vgui_observable const*) { update(); } // ignore the observable
};

int main(int argc, char **argv)
{
  vgui::init(argc, argv);

  // Create a deck tableau and add images given in the parameter list:
  vgui_deck_tableau_new deck;
  unsigned int w = 0, h = 0;
  for (int i=1; i<argc; ++i)
  {
    if (vul_file::exists(argv[i]))
    {
      vgui_image_tableau_new t(argv[i]);
      if (t->width ()>w) w = t->width ();
      if (t->height()>h) h = t->height();
      deck->add(t);
    }
    else
      vcl_cerr << "no such file : \'" << argv[i] << "\'\n";
  }
  deck->index(0);

  // Make an observer of the deck. It will set the window title
  // every time the deck's observers are notified.
  new example_window_title_setter(argv, deck);

  vgui_viewer2D_tableau_new viewer(deck);
  vgui_shell_tableau_new shell(viewer);
  shell->set_enable_key_bindings(true);

  return vgui::run(shell, w, h);
}
