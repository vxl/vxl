// mouse-draw-line.cxx
//
// This example a line is drawn by clicking on the end points.

#include <vgui/vgui.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D.h>
#include <vgui/vgui_easy2D.h>
#include <vgui/vgui_shell_tableau.h>

struct example_tableau : public vgui_easy2D
{
  float start_x;
  float start_y;

  example_tableau(vgui_image_tableau_sptr imtab) :
    vgui_easy2D(imtab)
    {
      start_x = -1;
      start_y = -1;
    }
  ~example_tableau() { }

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
        start_x = start_y = -1;
      }
      vcl_cout << "selecting at " << e.wx << " " << e.wy << vcl_endl;
      return true; // event has been used
    }

    // We are not interested in other events, so pass event to base class:
    return vgui_easy2D::handle(e);
  }
};

struct example_tableau_new : public vgui_easy2D_sptr {
  // is this naughty? perhaps it is.
  example_tableau_new(vgui_image_tableau_sptr const& i) : vgui_easy2D_sptr(new example_tableau(i)) { }
};

int main(int argc,char **argv)
{
  vgui::init(argc, argv);

  // Load an image into an image tableau:
  vgui_image_tableau_new image(argc>1 ? argv[1] : "az32_10.tif");

  // Put the image tableau inside our tableau:
  example_tableau_new example_tab(image);

  vgui_viewer2D_new viewer(example_tab);
  vgui_shell_tableau_new shell(viewer);

  // Create a window, add the tableau and show it on screen:
  return vgui::run(shell, 512, 512);
}
