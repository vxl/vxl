#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include "brct_windows_frame.h"
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>

int main(int argc, char **argv)
{
  vgui::init(argc, argv);
  vcl_string title = "Brown reconstruction demo";
  vgui_menu menubar; // this line will be deprecated soon.
  vgui_window* win = vgui::produce_window(800, 600, menubar, title);
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  win->get_adaptor()->set_tableau(brct_windows_frame::instance());
  brct_windows_frame::instance()->set_window(win);
  brct_windows_frame::instance()->post_redraw();
  return vgui::run();
}
