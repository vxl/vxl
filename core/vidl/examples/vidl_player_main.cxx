// This is core/vidl/example/vidl_player_main.cxx
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vidl/vidl_io.h>
#include "vidl_player_menus.h"
#include "vidl_player_manager.h"

#include <vidl/vidl_image_list_codec.h>

#ifdef HAS_MPEG2
# include <vidl/vidl_mpegcodec.h>
#endif

#ifdef VCL_WIN32
#include <vidl/vidl_avicodec.h>
#endif

int main(int argc, char** argv)
{
  // Register video codecs
  vidl_io::register_codec(new vidl_image_list_codec);

#ifdef VCL_WIN32
  vidl_io::register_codec(new vidl_avicodec);
#endif

#ifdef HAS_MPEG2
  vcl_cout << " Has MPEG\n";
  vidl_io::register_codec(new vidl_mpegcodec);
#endif

   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = vidl_player_menus::get_menu();
  unsigned w = 640, h = 480;

  vcl_string title = "VIDL Video Player";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(vidl_player_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  vidl_player_manager::instance()->set_window(win);
  vidl_player_manager::instance()->post_redraw();

  return  vgui::run();
}
