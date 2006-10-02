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
  SEGV_VIL_DLL_DATA static void remove_image_callback();
  SEGV_VIL_DLL_DATA static void convert_to_grey_callback();
  SEGV_VIL_DLL_DATA static void set_range_params_callback();
  SEGV_VIL_DLL_DATA static void clear_display_callback();
  SEGV_VIL_DLL_DATA static void display_images_as_color_callback();
  SEGV_VIL_DLL_DATA static void threshold_image_callback();
  SEGV_VIL_DLL_DATA static void harris_corners_callback();
  SEGV_VIL_DLL_DATA static void nonmaximal_suppression_callback();
  SEGV_VIL_DLL_DATA static void vd_edges_callback();
  SEGV_VIL_DLL_DATA static void fit_lines_callback();
  SEGV_VIL_DLL_DATA static void fit_conics_callback();
  SEGV_VIL_DLL_DATA static void fit_overlay_conics_callback();
  SEGV_VIL_DLL_DATA static void regions_callback();
  SEGV_VIL_DLL_DATA static void rotate_image_callback();
  SEGV_VIL_DLL_DATA static void reduce_image_callback();
  SEGV_VIL_DLL_DATA static void expand_image_callback();
  SEGV_VIL_DLL_DATA static void flip_image_lr_callback();
  SEGV_VIL_DLL_DATA static void add_images_callback();
  SEGV_VIL_DLL_DATA static void subtract_images_callback();
  SEGV_VIL_DLL_DATA static void negate_image_callback();
  SEGV_VIL_DLL_DATA static void entropy_callback();
  SEGV_VIL_DLL_DATA static void minfo_callback();
  SEGV_VIL_DLL_DATA static void max_trace_scale_callback();
  SEGV_VIL_DLL_DATA static void inline_viewer_callback();
  SEGV_VIL_DLL_DATA static void intensity_profile_callback();
  SEGV_VIL_DLL_DATA static void intensity_histogram_callback();


  SEGV_VIL_DLL_DATA static vgui_menu get_menu();
};

#endif // segv_vil_menus_h_
