#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <bmvl/bmvv/bmvv_tomg_menus.h>
#include <bmvl/bmvv/bmvv_tomography_manager.h>

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = bmvv_tomg_menus::get_menu();
  unsigned w = 400, h = 340;

  vcl_string title = "MODTOM";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(bmvv_tomography_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  bmvv_tomography_manager::instance()->set_window(win);
  bmvv_tomography_manager::instance()->post_redraw();

  return  vgui::run();
}
