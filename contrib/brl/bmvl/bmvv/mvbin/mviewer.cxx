#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <bmvl/bmvv/bmvv_menus.h>
#include <bmvl/bmvv/bmvv_multiview_manager.h>

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = bmvv_menus::get_menu();
  unsigned w = 640, h = 360;

  vcl_string title = "MVIEWER";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(bmvv_multiview_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  bmvv_multiview_manager::instance()->set_window(win);
  bmvv_multiview_manager::instance()->post_redraw();

  // Custom instructions
//bmvv_multiview_manager::instance()->load_image_file("C:\\user\\bazin\\data\\rondpoint\\rond_point020.pgm", true, 0,0);
//bmvv_multiview_manager::instance()->load_image_file("C:\\user\\bazin\\data\\rondpoint\\rond_point021.pgm", true, 1,0);
//bmvv_multiview_manager::instance()->track_edges();

  return  vgui::run();
}
