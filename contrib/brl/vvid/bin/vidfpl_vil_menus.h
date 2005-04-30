#ifndef vidfpl_vil_menus_h_
#define vidfpl_vil_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief the setup for vidfpl_vil_menus for the video file player
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy April 27,  2005    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------

class vgui_menu;

//: Menu callbacks are represented as static methods
//  The key method is ::get_menu, which does the work of
//  assembling the menu bar for the main executable 
class vidfpl_vil_menus
{
 public:
  static void load_video_callback();
  static void start_frame_callback();
  static void end_frame_callback();
  static void play_video_callback();
  static void pause_video_callback();
  static void next_frame_callback();
  static void prev_frame_callback();
  static void stop_video_callback();
  static void set_range_params_callback();
  static void quit_callback();
  static vgui_menu get_menu();
 private:
  vidfpl_vil_menus() {}
};

#endif // vidfpl_vil_menus_h_
