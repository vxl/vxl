#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vsrl/vsrl_menus.h>
#include <vsrl/vsrl_manager.h>

int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  vgui_menu menubar = vsrl_menus::get_menus();
  unsigned w=512, h=512;
  vcl_string title = "Dense Matcher Test Program";
  vgui_window* vwin = vgui::produce_window(w, h, menubar, title);
  vsrl_manager* vman = vsrl_manager::instance();
  vwin->get_adaptor()->set_tableau(vman);
  vwin->set_statusbar(true);
  vwin->show();
  return vgui::run();
}
