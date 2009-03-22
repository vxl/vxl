// This is core/vidl/examples/vidl_player_menus.h
#ifndef vidl_player_menus_h_
#define vidl_player_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \author Matt Leotta
// \brief The setup for vidl_player_menus for the vidl video player
//
//  This is essentially a namespace for static methods;
//  there is no public constructor.  Refer to "10.7 Building an Application"
//  in the VXL Book.
//
//--------------------------------------------------------------------------------
class vgui_menu;

//: Menu callbacks are represented as static methods
//  The key method is ::get_menu, which does the work of
//  assembling the menu bar for the main executable
class vidl_player_menus
{
 public:
  static void load_video_callback();
  static void quit_callback();

  static void play_video_callback();
  static void pause_video_callback();
  static void next_frame_callback();
  static void prev_frame_callback();
  static void go_to_frame_callback();
  static void stop_video_callback();

  static vgui_menu get_menu();

 private:
  vidl_player_menus() {}
};

#endif // vidl_player_menus_h_
