#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <segv/segv_menus.h>
#include <segv/segv_segmentation_manager.h>

#ifdef HAS_X11
# include <vgui/internals/vgui_accelerate_x11.h>
#endif

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = segv_menus::get_menu();
  unsigned w = 400, h = 340;
   vcl_string title = "SEG";
   vgui_window* win = vgui::produce_window(w, h, menubar, title);
   segv_segmentation_manager* segm = segv_segmentation_manager::instance();
   segm->set_window(win);
   win->get_adaptor()->set_tableau(segm);
   win->set_statusbar(true);
   win->enable_vscrollbar(true);
   win->enable_hscrollbar(true);
   win->show();
  return  vgui::run();
}
