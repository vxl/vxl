#ifndef vidster_menus_h_
#define vidster_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief menus
//   the setup for menus for vidster
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 7, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
class vidster_menus
{
 public:
  static void quit_callback();
  static void set_camera_params_callback();
  static void start_live_video_callback();
  static void stop_live_video_callback();
  static vgui_menu get_menu();
 private:
  vidster_menus(){};
};
#endif // vidster_menus_h_
