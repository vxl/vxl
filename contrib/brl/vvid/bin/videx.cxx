#include <vcl_ios.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_exit()

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include <vidl/vidl_io.h>
#include "videx_menus.h"
#include <vvid/vvid_live_video_manager.h>

#ifdef HAS_MPEG
# include <vidl/vidl_mpegcodec.h>
#endif

#ifdef HAS_X11
# include <vgui/internals/vgui_accelerate_x11.h>
#endif

#ifdef VCL_WIN32
#include <vidl/vidl_avicodec.h>
#endif

int main(int argc, char** argv)
{
  // Register video codecs
#ifdef VCL_WIN32
  vidl_io::register_codec(new vidl_avicodec);
#endif

#ifdef HAS_MPEG
  vidl_io::register_codec(new vidl_mpegcodec);
#endif

   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = videx_menus::get_menu();
  unsigned w = 400, h = 340;

  vcl_string title = "VIDEX";
  vgui_window* win = vgui::produce_window(w, h, menubar, title);
  win->get_adaptor()->set_tableau(vvid_live_video_manager::instance());
  win->set_statusbar(true);
  win->enable_vscrollbar(true);
  win->enable_hscrollbar(true);
  win->show();
  vvid_live_video_manager::instance()->set_window(win);
  vvid_live_video_manager::instance()->post_redraw();
  
  return  vgui::run();
}
