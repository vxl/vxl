// This example displays an image with a popup-menu.

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>

// Set up a dummy callback function for the menu to call (for
// simplicity all menu items will call this function):
static void dummy()
{
  std::cerr << "Dummy function called" << std::endl;
}

// Create a vgui.menu:
vgui_menu create_menus()
{
  vgui_menu file;
  file.add("Open", dummy);
  file.add("Quit", dummy);

  vgui_menu image;
  image.add("Center image", dummy);
  image.add("Show histogram", dummy);

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
    std::cerr << "Please give an image filename on the command line" << std::endl;
    return 0;
  }

  // Load an image into an image tableau:
  vgui_image_tableau_new image(argv[1]);
  vgui_viewer2D_tableau_new viewer(image);
  vgui_shell_tableau_new shell(viewer);

  // Create a window and add the tableau:
  vgui_window *win = vgui::produce_window(512, 512);
  win->get_adaptor()->set_tableau(shell);

  // Add our menu items to the base pop-up (this menu appears when the
  // user clicks the right mouse button on the tableau:
  win->get_adaptor()->include_in_popup(create_menus());
  win->get_adaptor()->bind_popups();
  win->show();
  return vgui::run();
}
