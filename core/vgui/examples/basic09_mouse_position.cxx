// This displays the mouse position when the left mouse button is pressed.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//-----------------------------------------------------------------------------
// A tableau that displays the mouse position when left mouse button is pressed.
struct example_tableau : public vgui_image_tableau
{
  example_tableau(char const *f) : vgui_image_tableau(f){ }

  ~example_tableau() { }

  bool handle(const vgui_event &e)
  {
    if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == 0)
    {
      std::cout << "selecting at " << e.wx << ' ' << e.wy << std::endl;
      return true; // event has been used
    }

    // We are not interested in other events, so pass event to base class:
    return vgui_image_tableau::handle(e);
  }
};

//-----------------------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct example_tableau_new : public vgui_image_tableau_sptr
{
  example_tableau_new(char const *f) : vgui_image_tableau_sptr(
    new example_tableau(f)) { }
};

//-----------------------------------------------------------------------------
// The first command line argument is expected to be an image filename.
int main(int argc,char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    std::cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Load an image into my tableau (derived from vgui_image_tableau)
  vgui_tableau_sptr my_tab = example_tableau_new(argv[1]);

  vgui_viewer2D_tableau_new viewer(my_tab);
  vgui_shell_tableau_new shell(viewer);

  // Start event loop, using easy method.
  return vgui::run(shell, 512, 512);
}
