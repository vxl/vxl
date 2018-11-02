#ifndef bwm_image_proc_h_
#define bwm_image_proc_h_

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_polygon.h>

#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_range_map_params_sptr.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>

#include <vdgl/vdgl_digital_curve_sptr.h>

class bwm_image_processor
{
 public:
  static void hist_plot(bgui_image_tableau_sptr img,
                        vsol_polygon_2d_sptr p = nullptr);

  static void intensity_profile(bgui_image_tableau_sptr img,
                                float start_col, float start_row,
                                float end_col, float end_row);

  static void range_map(bgui_image_tableau_sptr img);

#if 0
  static bool step_edges_vd(bgui_image_tableau_sptr const& img,
                            vsol_box_2d_sptr const& roi,
                            std::vector<vdgl_digital_curve_sptr>& edges);
#endif

  static bool step_edges_vd(bgui_image_tableau_sptr const& img,
                            vsol_box_2d_sptr const& roi,
                            std::vector<vsol_digital_curve_2d_sptr>& edges);

  static bool lines_vd(bgui_image_tableau_sptr const& img,
                       vsol_box_2d_sptr const& roi,
                       std::vector<vsol_line_2d_sptr>& edges);

  static void scan_regions(bgui_image_tableau_sptr const& img,
                           std::vector<vgl_polygon<double> > const& regions);

  static bool crop_to_box(bgui_image_tableau_sptr const& img,
                          vsol_box_2d_sptr const& roi,
                          vil_image_resource_sptr& chip);

};

#endif
