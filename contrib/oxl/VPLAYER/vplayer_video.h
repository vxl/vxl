#ifndef vplayer_video_h_
#define vplayer_video_h_

//--------------------------------------------------------------------------------
// .NAME	vplayer_video
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
  static void play_video(const void *);
  static void stop_video(const void *);
  static void go_to_frame(const void *);
  static void next_frame(const void *);
  static void prev_frame(const void *);
  static void set_speed(const void *);
  static vgui_menu create_video_menu();
private:
};

#endif // vplayer_video_
