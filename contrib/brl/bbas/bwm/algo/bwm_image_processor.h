#ifndef bwm_image_proc_h_
#define bwm_image_proc_h_

#include <vcl_vector.h>

#include <vgl/vgl_polygon.h>

#include <bgui/bgui_image_tableau.h>
#include <vgui/vgui_range_map_params_sptr.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>

#include <vdgl/vdgl_digital_curve_sptr.h>

class bwm_image_processor {
public:
  static void hist_plot(bgui_image_tableau_sptr img);

  static void intensity_profile(bgui_image_tableau_sptr img,
                                float start_col, float end_col, 
                                float start_row, float end_row);


  static void range_map(bgui_image_tableau_sptr img);

#if 0
  static bool step_edges_vd(bgui_image_tableau_sptr const& img,
                            vsol_box_2d_sptr const& roi,
                            vcl_vector<vdgl_digital_curve_sptr>& edges);

#endif
  static bool step_edges_vd(bgui_image_tableau_sptr const& img,
                            vsol_box_2d_sptr const& roi,
                            vcl_vector<vsol_digital_curve_2d_sptr>& edges);

  static bool lines_vd(bgui_image_tableau_sptr const& img,
                       vsol_box_2d_sptr const& roi,
                       vcl_vector<vsol_line_2d_sptr>& edges);

  static void scan_regions(bgui_image_tableau_sptr const& img,
                           vcl_vector<vgl_polygon<double> > const& regions);

};

#endif
