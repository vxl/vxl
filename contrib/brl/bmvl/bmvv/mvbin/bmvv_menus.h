#ifndef bmvv_menus_h_
#define bmvv_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief menus
//   the setup for menus for bmvv
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 11, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
class bmvv_menus
{
 public:
  static void quit_callback();
  static void load_image_callback();
  static void clear_display_callback();
  static void vd_edges_callback();
#ifdef HAS_XERCES
  static void read_xml_edges_callback();
#endif
  static void show_epipolar_line_callback();
  static vgui_menu get_menu();
 private:
  bmvv_menus(){};
};
#endif // bmvv_menus_h_
