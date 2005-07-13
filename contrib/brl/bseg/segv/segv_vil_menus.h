#ifndef segv_vil_menus_h_
#define segv_vil_menus_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for segv_vil
//  This is essentially a namespace for static methods;
//  there is no public constructor - cloned from segv_vil_menus to support vil
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 28, 2004    Initial version.
// \endverbatim
//----------------------------------------------------------------------------
#include <segv/vil_dll.h>
class vgui_menu;

class segv_vil_menus
{
  //: inaccessible default constructor: all methods are static
  segv_vil_menus() {}
 public:
  SEGV_VIL_DLL_DATA static void quit_callback();
  SEGV_VIL_DLL_DATA static void load_image_callback();
  SEGV_VIL_DLL_DATA static void save_image_callback();
  SEGV_VIL_DLL_DATA static void set_range_params_callback();
  SEGV_VIL_DLL_DATA static void clear_display_callback();
  SEGV_VIL_DLL_DATA static void threshold_image_callback();
  SEGV_VIL_DLL_DATA static void harris_corners_callback();
  SEGV_VIL_DLL_DATA static void vd_edges_callback();
  SEGV_VIL_DLL_DATA static void fit_lines_callback();
  SEGV_VIL_DLL_DATA static void fit_conics_callback();
  SEGV_VIL_DLL_DATA static void regions_callback();
  //test/development do not use
  SEGV_VIL_DLL_DATA static void test_inline_viewer_callback();
  SEGV_VIL_DLL_DATA static void test_ellipse_draw_callback();


  SEGV_VIL_DLL_DATA static vgui_menu get_menu();
};

#endif // segv_vil_menus_h_
