// This is core/vidl/examples/vidl_player_main.cxx
#include "vidl_player_menus.h"
#include "vidl_player_manager.h"
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vidl/vidl_io.h>

int main(int argc, char** argv)
{
   // Initialize the toolkit.
  vgui::init(argc, argv);
  vgui_menu menubar = vidl_player_menus::get_menu();
  unsigned w = 640, h = 480;

  vcl_cout << "-- Registered codecs --\n";
  vcl_list<vcl_string> types = vidl_io::supported_types();
  for ( vcl_list<vcl_string>::iterator t = types.begin();
        t != types.end(); ++t )
    vcl_cout << *t << vcl_endl;
  vcl_cout << "-----------------------\n";

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
