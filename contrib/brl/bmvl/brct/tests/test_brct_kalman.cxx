#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgui/vgui.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include "brct_windows_frame.h"
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgl/vgl_point_3d.h>
#include "brct_menus.h"
int main(int argc, char **argv)
{
  vgui::init(argc, argv);
 
	// set up components of window: menu, scroll bar etc.
  vcl_string title = "Brown Reconstruction Demo";

  vgui_window* win = vgui::produce_window(800, 600, brct_menus::get_menu(), title);
  win->get_adaptor()->set_tableau(brct_windows_frame::instance());
/*  win->get_adaptor()->bind_popups(vgui_CTRL);  
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
*/
 //brct_windows_frame::instance()->set_window(win);
 //brct_windows_frame::instance()->post_redraw();

  return vgui::run();
 
 // return vgui::run(brct_windows_frame::instance(), 800, 600, title);
}
