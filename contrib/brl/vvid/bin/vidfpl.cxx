#include <vcl_compiler.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vidl_vil1/vidl_vil1_io.h>
#include "vidfpl_menus.h"
#include <vvid/vvid_file_manager.h>

#include <vidl_vil1/vidl_vil1_image_list_codec.h>

#ifdef HAS_MPEG2
# include <vidl_vil1/vidl_vil1_mpegcodec.h>
#endif

#ifdef VCL_WIN32
#include <vidl_vil1/vidl_vil1_avicodec.h>
#endif

int main(int argc, char** argv)
{
  // Register video codecs
  vidl_vil1_io::register_codec(new vidl_vil1_image_list_codec);

#ifdef VCL_WIN32
  vidl_vil1_io::register_codec(new vidl_vil1_avicodec);
#endif

#ifdef HAS_MPEG2
  vcl_cout << " Has MPEG\n";
  vidl_vil1_io::register_codec(new vidl_vil1_mpegcodec);
#endif
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = vidfpl_menus::get_menu();
  unsigned w = 400, h = 340;

  vcl_string title = "Video File Player";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(vvid_file_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  vvid_file_manager::instance()->set_window(win);
  vvid_file_manager::instance()->post_redraw();

  return  vgui::run();
}
