// This is contrib/brl/bbas/vidl2/examples/vidl2_player_menus.h
#ifndef vidl2_player_menus_h_
#define vidl2_player_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \author Matt Leotta
// \brief The setup for vidl2_player_menus for the vidl2 video player
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
class vidl2_player_menus
{
  public:
    static void quit_callback();

    static void open_image_list_istream_callback();
    static void open_image_list_ostream_callback();
#ifdef HAS_FFMPEG
    static void open_ffmpeg_istream_callback();
    static void open_ffmpeg_ostream_callback();
#endif
#ifdef HAS_DC1394
    static void open_dc1394_istream_callback();
#endif
    static void close_istream_callback();
    static void close_ostream_callback();
    static void pipe_streams_callback();

    static void play_video_callback();
    static void pause_video_callback();
    static void next_frame_callback();
    static void prev_frame_callback();
    static void go_to_frame_callback();
    static void stop_video_callback();

    static vgui_menu get_menu();

  private:
    vidl2_player_menus() {}
};

#endif // vidl2_player_menus_h_
