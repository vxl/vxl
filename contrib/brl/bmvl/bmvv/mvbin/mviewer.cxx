#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <bmvl/bmvv/bmvv_menus.h>
#include <bmvl/bmvv/bmvv_multiview_manager.h>

#ifdef HAS_X11
# include <vgui/internals/vgui_accelerate_x11.h>
#endif

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = bmvv_menus::get_menu();
  unsigned w = 400, h = 340;

  vcl_string title = "MVIEWER";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(bmvv_multiview_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  bmvv_multiview_manager::instance()->set_window(win);
  bmvv_multiview_manager::instance()->post_redraw();

  return  vgui::run();
}
