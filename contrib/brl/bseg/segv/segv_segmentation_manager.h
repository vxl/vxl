#ifndef segv_segmentation_manager_h_
#define segv_segmentation_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Manager for segmentation algorithm execution and display
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy November 18, 2002    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <vgui/vgui_rubberband_tableau_sptr.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <bgui/bgui_bargraph_clipon_tableau_sptr.h>

class vgui_window;

class segv_segmentation_manager : public vgui_wrapper_tableau
{
 public:
  segv_segmentation_manager();
  ~segv_segmentation_manager();
  static segv_segmentation_manager *instance();
  void quit();
  void load_image();
  void clear_display();
  void original_image();
  void roi();
  void gaussian();
  void convolution();
  void downsample();
  void harris_measure();
  void beaudet_measure();
  void vd_edges();
  void regions();
  void test_camera_parms();
  void fit_lines();
  void test_face();
  void test_digital_lines();
  void display_IHS();
  void rotate_image();
  void compute_mutual_info();
  void create_box();
  void create_polygon();
  void compute_background_info();
  void set_foreground_face();
  void set_background_face();
  void compute_parallel_coverage();
  void compute_watershed_regions();
  void find_vehicle();
  void display_histogram_track();
  void set_range_params();

#if 0
#ifdef HAS_XERCES
  void read_xml_edges();
#endif
#endif

  void init();

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

 protected:
  void draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges, bool verts=false);
  void draw_lines(vcl_vector<vsol_line_2d_sptr> const & line_segs,
                  const vgui_style_sptr& style = NULL);
  void draw_polylines(vcl_vector<vsol_polyline_2d_sptr> const & polys);
  void draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
                    bool verts=false);
  void draw_points(vcl_vector<vsol_point_2d_sptr> const & points,
                   const vgui_style_sptr& style);

  void set_selected_grid_image(vil1_image& image);
  void add_image(vil1_image& image);
  vil1_image selected_image();
  vil1_image image_at(const unsigned col, const unsigned row);
  bool
  set_image_at(const unsigned col, const unsigned row, vil1_image& image);
  bgui_image_tableau_sptr selected_image_tab();
  bgui_vtol2D_tableau_sptr selected_vtol2D_tab();
  bgui_picker_tableau_sptr selected_picker_tab();
  vgui_rubberband_tableau_sptr selected_rubber_tab();
  vtol_face_2d_sptr face_at(const int col, const int row);

 private:
  //flags
  bool first_; //first image load
  vgui_window* win_;
  vgui_grid_tableau_sptr grid_;
  vtol_face_2d_sptr foreground_face_;
  vtol_face_2d_sptr background_face_;
  bgui_bargraph_clipon_tableau_sptr bargraph_;
  static segv_segmentation_manager *instance_;
};

#endif // segv_segmentation_manager_h_
