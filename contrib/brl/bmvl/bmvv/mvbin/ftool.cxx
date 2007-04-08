#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <bmvl/bmvv/bmvv_f_menus.h>
#include <bmvl/bmvv/bmvv_f_manager.h>

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = bmvv_f_menus::get_menu();
  unsigned w = 640, h = 360;

  vcl_string title = "FTOOL";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(bmvv_f_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  bmvv_f_manager::instance()->set_window(win);
  bmvv_f_manager::instance()->post_redraw();

  vgui::run();
  return 0;
}
