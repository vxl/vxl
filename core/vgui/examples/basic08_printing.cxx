// This example shows saving the OpenGL area to a PostScript file and to
// an image file.  In the PostScript file all the objects will be saved
// separately from the image (look inside the .ps file to see what I mean).

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_menu.h>

vgui_easy2D_tableau_sptr easy2D;

static void print_ps()
{
  // Printing to a post-script file:
  easy2D->print_psfile("kymfile.ps", 1, true);
  std::cout << "Attempting to print to kymfile.ps" << std::endl;
}

static void save_jpg()
{
  // Saving the OpenGL area as a jpeg image:
  vgui_utils::dump_colour_buffer("kymfile.jpg");
  std::cout << "Attempting to print to kymfile.jpg" << std::endl;
}

vgui_menu create_menu()
{
  vgui_menu file;
  file.add("Print PostScript", print_ps);
  file.add("Save jpeg", save_jpg);

  vgui_menu bar;
  bar.add("File", file);
  return bar;
}

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  if (argc <= 1)
  {
    std::cerr << "Please give an image filename on the command line" << std::endl;
     return 1;
  }

  // Make something interesting to print (an image with some 2d objects on).
  vgui_image_tableau_new image(argv[1]);
  easy2D = vgui_easy2D_tableau_new(image);
  easy2D->set_foreground(0,1,0);
  easy2D->set_point_radius(5);
  easy2D->add_point(10, 20);
  easy2D->set_foreground(0,0,1);
  easy2D->set_line_width(2);
  easy2D->add_line(100,100,200,400);
  easy2D->add_circle(100, 100, 50);
  vgui_viewer2D_tableau_new viewer(easy2D);
  vgui_shell_tableau_new shell(viewer);

  return vgui::run(shell, image->width(), image->height(), create_menu());
}
