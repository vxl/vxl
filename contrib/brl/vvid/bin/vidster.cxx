#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include "vidster_menus.h"
#include <vvid/vvid_live_stereo_manager.h>

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = vidster_menus::get_menu();
  unsigned w = 400, h = 340;

  vcl_string title = "VIDSTER";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(vvid_live_stereo_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  vvid_live_stereo_manager::instance()->set_window(win);
  vvid_live_stereo_manager::instance()->post_redraw();

  return  vgui::run();
}
