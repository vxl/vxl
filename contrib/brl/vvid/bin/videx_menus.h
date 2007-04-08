#ifndef videx_menus_h_
#define videx_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for videx
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy October 7, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vgui/vgui_menu.h>
class videx_menus
{
 public:
  static void quit_callback();
  static void set_camera_params_callback();
  static void set_detection_params_callback();
  static void no_op_callback();
  static void start_live_video_callback();
  static void stop_live_video_callback();
  static void toggle_histogram_callback();
  static void capture_sequence_callback();
  static void init_capture_callback();
  static void stop_capture_callback();
  static void reset_camera_link_callback();
  static vgui_menu get_menu();
 private:
  videx_menus() {}
};
#endif // videx_menus_h_
