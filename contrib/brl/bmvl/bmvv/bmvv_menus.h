#ifndef bmvv_menus_h_
#define bmvv_menus_h_
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
#include <bmvv/dll.h>
class bmvv_menus
{
 public:
BMVV_DLL_DATA  static void quit_callback();
  BMVV_DLL_DATA static void load_image_callback();
  BMVV_DLL_DATA static void clear_display_callback();
  BMVV_DLL_DATA static void clear_selected_callback();
  BMVV_DLL_DATA static void vd_edges_callback();
#ifdef HAS_XERCES
  BMVV_DLL_DATA static void read_xml_edges_callback();
#endif
  BMVV_DLL_DATA static void show_epipolar_line_callback();
  BMVV_DLL_DATA static void select_curve_corres_callback();
  BMVV_DLL_DATA static vgui_menu get_menu();
 private:
  bmvv_menus(){};
};
#endif // bmvv_menus_h_
