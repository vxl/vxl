// mouse-position.cc
//
// This example displays the mouse position when the left mouse button is pressed.

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_shell_tableau.h>

struct example_tableau : public vgui_image_tableau
{
  example_tableau(char const *f) : vgui_image_tableau(f){ }
  ~example_tableau() { }

  bool handle(const vgui_event &e)
    {
    if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == 0)
    {
      vcl_cout << "selecting at " << e.wx << " " << e.wy << vcl_endl;
      return true; // event has been used
    }

    // We are not interested in other events, so pass event to base class:
    return vgui_image_tableau::handle(e);
  }
};

int main(int argc,char **argv)
{
  vgui::init(argc, argv);

  // Load an image into our tableau:
  vgui_tableau_sptr my_tab = new example_tableau(argc>1 ? argv[1] : "az32_10.tif");

  // Put the tableau inside a 2D viewer tableau:
  vgui_viewer2D_new viewer(my_tab);
  vgui_shell_tableau_new shell(viewer);

  // Start event loop, using easy method.
  return vgui::run(shell, 512, 512);
}
