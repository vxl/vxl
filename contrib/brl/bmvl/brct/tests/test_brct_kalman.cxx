#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include "brct_windows_frame.h"
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>


int main(int argc, char **argv)
{
  vgui::init(argc, argv);
 
  vcl_string title = "Brown Reconstruction Demo";
  brct_windows_frame::instance(title);
  return vgui::run();
}
