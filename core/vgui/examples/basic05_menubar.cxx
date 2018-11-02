// This example displays an image with a menu bar.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

// Set up a dummy callback function for the menu to call (for
// simplicity all menu items will call this function):
static void dummy()
{
  std::cerr << "Dummy function called\n";
}

// Create a vgui menu:
vgui_menu create_menus()
{
  vgui_menu file;
  file.add("Open",dummy,(vgui_key)'O',vgui_CTRL);
  file.add("Quit",dummy,(vgui_key)'R',vgui_SHIFT);

  vgui_menu image;
  image.add("Center image",dummy);
  image.add("Show histogram",dummy);

  vgui_menu bar;
  bar.add("File",file);
  bar.add("Image",image);

  return bar;
}

int main(int argc, char ** argv)
{
  vgui::init(argc,argv);
  if (argc <= 1)
  {
    std::cerr << "Please give an image filename on the command line\n";
    return 0;
  }

  // Make our tableau hierarchy.
  vgui_image_tableau_new image(argv[1]);
  vgui_viewer2D_tableau_new viewer(image);
  vgui_shell_tableau_new shell(viewer);

  // Create a window, but this time we also pass in a vgui_menu.
  return vgui::run(shell, 512, 512, create_menus());
}
