#ifndef segv_menus_h_
#define segv_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief menus
//   the setup for menus for segv
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 18, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
class segv_menus
{
 public:
  static void quit_callback();
  static void load_image_callback();
  static void clear_display_callback();
  static void vd_edges_callback();
  static void clean_vd_edges_callback();
  static vgui_menu get_menu();
 private:
  segv_menus(){};
};
#endif // segv_menus_h_
