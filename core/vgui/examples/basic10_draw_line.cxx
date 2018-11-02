// In this example a line is drawn by clicking on the end points.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

//----------------------------------------------------------------------------
struct example_tableau : public vgui_easy2D_tableau
{
  // Start point of the line
  float start_x;
  float start_y;

  // Constructor
  example_tableau(vgui_image_tableau_sptr imtab) :
    vgui_easy2D_tableau(imtab)
    {
      start_x = -1;
      start_y = -1;
    }

  // Destructor
  ~example_tableau() { }

  // Receives all events passed to this tableau.
  // We are interested in mouse button down events defining the start and
  // end points of the line.
  bool handle(const vgui_event &e)
  {
    if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == 0)
    {
      // Convert the coordinates from viewport coords to image coords:
      vgui_projection_inspector pi;
      float ix, iy;
      pi.window_to_image_coordinates(int(e.wx),int(e.wy), ix,iy);

      if (start_x == -1)
      {
        start_x = ix;
        start_y = iy;
      }
      else
      {
        this->set_line_width(3);
        this->add_line(start_x, start_y, ix, iy);
        std::cout << "Adding line to display" << std::endl;
        this->post_redraw();
        start_x = start_y = -1;
      }
      std::cout << "selecting at " << e.wx << " " << e.wy << std::endl;
      return true; // event has been used
    }

    // We are not interested in other events, so pass event to base class:
    return vgui_easy2D_tableau::handle(e);
  }
};

//----------------------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct example_tableau_new : public vgui_easy2D_tableau_sptr
{
  example_tableau_new(vgui_image_tableau_sptr const& i) : vgui_easy2D_tableau_sptr(new example_tableau(i)) { }
};

//----------------------------------------------------------------------------
int main(int argc,char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    std::cerr << "Please give an image filename on the command line" << std::endl;
    return 0;
  }

  vgui_image_tableau_new image(argv[1]);

  // Put the image tableau inside our tableau (derived from vgui_easy2D_tableau):
  example_tableau_new example_tab(image);

  vgui_viewer2D_tableau_new viewer(example_tab);
  vgui_shell_tableau_new shell(viewer);

  return vgui::run(shell, 512, 512);
}
