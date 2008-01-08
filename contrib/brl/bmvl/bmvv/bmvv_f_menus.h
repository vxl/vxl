#ifndef bmvv_f_menus_h_
#define bmvv_f_menus_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for bmvv f manager
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy February 19, 2007    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------

#include <bmvv/dll.h>
class vgui_menu;

class bmvv_f_menus
{
 public:
  BMVV_DLL_DATA  static void quit_callback();
  BMVV_DLL_DATA static void load_image_callback();
  BMVV_DLL_DATA static void load_image_and_cam_callback();
  BMVV_DLL_DATA static void save_image_callback();
  BMVV_DLL_DATA static void clear_display_callback();
  BMVV_DLL_DATA static void read_corrs_callback();
  BMVV_DLL_DATA static void save_corrs_callback();
  BMVV_DLL_DATA static void display_corrs_callback();
  BMVV_DLL_DATA static void load_world_callback();
  BMVV_DLL_DATA static void save_world_callback();
  BMVV_DLL_DATA static void project_world_callback();
  BMVV_DLL_DATA static void read_f_matrix_callback();
  BMVV_DLL_DATA static void save_f_matrix_callback();
  BMVV_DLL_DATA static void display_right_epi_lines_callback();
  BMVV_DLL_DATA static void display_picked_epi_line_callback();
  BMVV_DLL_DATA static void create_correspondence_callback();
  BMVV_DLL_DATA static void pick_correspondence_callback();
  BMVV_DLL_DATA static void compute_f_matrix_callback();
  BMVV_DLL_DATA static void reconstruct_world_callback();
  BMVV_DLL_DATA static void set_range_params_callback();
  BMVV_DLL_DATA static void intensity_profile_callback();
  BMVV_DLL_DATA static void intensity_histogram_callback();

  BMVV_DLL_DATA static vgui_menu get_menu();
 private:
  bmvv_f_menus() {}
};

#endif // bmvv_f_menus_h_
