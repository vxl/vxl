#ifndef segv_menus_h_
#define segv_menus_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for segv
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 18, 2002    Initial version.
// \endverbatim
//----------------------------------------------------------------------------
#include <segv/dll.h>
class vgui_menu;

class segv_menus
{
  //: inaccessible default constructor: all methods are static
  segv_menus() {}
 public:
  SEGV_DLL_DATA static void quit_callback();
  SEGV_DLL_DATA static void load_image_callback();
  SEGV_DLL_DATA static void clear_display_callback();
  SEGV_DLL_DATA static void original_image_callback();
  SEGV_DLL_DATA static void roi_callback();
  SEGV_DLL_DATA static void gaussian_callback();
  SEGV_DLL_DATA static void downsample_callback();
  SEGV_DLL_DATA static void convolution_callback();
  SEGV_DLL_DATA static void harris_measure_callback();
  SEGV_DLL_DATA static void beaudet_measure_callback();
  SEGV_DLL_DATA static void vd_edges_callback();
  SEGV_DLL_DATA static void regions_callback();
  SEGV_DLL_DATA static void fit_lines_callback();
  SEGV_DLL_DATA static void test_camera_parms_callback();
  SEGV_DLL_DATA static void display_IHS_callback();
  SEGV_DLL_DATA static void rotate_image_callback();
  SEGV_DLL_DATA static void compute_mutual_info_callback();
  SEGV_DLL_DATA static void compute_background_info_callback();
  SEGV_DLL_DATA static void create_box_callback();
  SEGV_DLL_DATA static void create_polygon_callback();
  SEGV_DLL_DATA static void set_foreground_face_callback();
  SEGV_DLL_DATA static void set_background_face_callback();
  SEGV_DLL_DATA static void compute_parallel_coverage_callback();
  SEGV_DLL_DATA static void compute_watershed_regions_callback();
  SEGV_DLL_DATA static void find_vehicle_callback();
  SEGV_DLL_DATA static void display_histogram_track_callback();
  SEGV_DLL_DATA static void set_range_params_callback();
  SEGV_DLL_DATA static void test_inline_viewer_callback();

#if 0
#ifdef HAS_XERCES
  SEGV_DLL_DATA static void read_xml_edges_callback();
#endif
#endif
  SEGV_DLL_DATA static void test_face_callback();
  SEGV_DLL_DATA static void test_digital_lines_callback();
  SEGV_DLL_DATA static vgui_menu get_menu();
};

#endif // segv_menus_h_
