#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include "basic_menus.h"
#include "basic_manager.h"

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  vgui_menu menubar = basic_menus::get_menu();
  unsigned w = 512, h = 512;
  vcl_string title = "REALLY BASIC";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  basic_manager* bas = basic_manager::instance();
  win->get_adaptor()->set_tableau(bas);
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  return  vgui::run();
}
