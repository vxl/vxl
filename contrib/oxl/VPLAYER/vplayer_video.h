#ifndef vplayer_video_h_
#define vplayer_video_h_

//--------------------------------------------------------------------------------
// .NAME    vplayer_video
// .SECTION Description:
//   Imlements video menu on the movie player
// .SECTION Author
//   Marko Bacic, Oxford RRG
// .SECTION Modifications:
//   Marko Bacic    05-SEP-2000    Initial version.
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
private:
};

#endif // vplayer_video_
