#ifndef vplayer_video_h_
#define vplayer_video_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Implements video menu on the movie player
// \author
//   Marko Bacic, Oxford RRG
// \date
//   05-SEP-2000
//--------------------------------------------------------------------------------

class vgui_menu;

class vplayer_video
{
 public:
  static void play_video();
  static void stop_video();
  static void go_to_frame();
  static void next_frame();
  static void prev_frame();
  static void set_speed();
  static vgui_menu create_video_menu();
};

#endif // vplayer_video_h_
