#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <bmvl/bmvv/bmvv_recon_menus.h>
#include <bmvl/bmvv/bmvv_recon_manager.h>

#ifdef HAS_X11
# include <vgui/internals/vgui_accelerate_x11.h>
#endif

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = bmvv_recon_menus::get_menu();
  unsigned w = 640, h = 360;

  vcl_string title = "RECON";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(bmvv_recon_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  bmvv_recon_manager::instance()->set_window(win);
  bmvv_recon_manager::instance()->post_redraw();

  return  vgui::run();
}
