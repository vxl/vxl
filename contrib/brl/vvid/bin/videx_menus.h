#ifndef videx_menus_h_
#define videx_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief menus
//   the setup for menus for videx
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 7, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
class videx_menus
{
 public:
  static void quit_callback();
  static void set_camera_params_callback();
  static void set_detection_params_callback();
  static void start_live_video_callback();
  static void stop_live_video_callback();
  static void capture_sequence_callback();
  static vgui_menu get_menu();
 private:
  videx_menus(){};
};
#endif // videx_menus_h_
