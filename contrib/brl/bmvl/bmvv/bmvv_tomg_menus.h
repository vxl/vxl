#ifndef bmvv_tomg_menus_h_
#define bmvv_tomg_menus_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief tomg_menus
//   the setup for tomg_menus for bmvv
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 11, 2002    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <bmvv/dll.h>
class bmvv_tomg_menus
{
 public:
BMVV_DLL_DATA  static void quit_callback();
  BMVV_DLL_DATA static void load_image_callback();
  BMVV_DLL_DATA static void save_sinogram_callback();
  BMVV_DLL_DATA static void clear_display_callback();
  BMVV_DLL_DATA static void clear_selected_callback();
  BMVV_DLL_DATA static void sinogram_callback();
  BMVV_DLL_DATA static vgui_menu get_menu();
 private:
  bmvv_tomg_menus(){};
};
#endif // bmvv_tomg_menus_h_
