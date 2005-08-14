#ifndef segv_vil_segmentation_manager_h_
#define segv_vil_segmentation_manager_h_
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
#include <vil/vil_image_resource.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <vgui/vgui_rubberband_tableau_sptr.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <bgui/bgui_bargraph_clipon_tableau_sptr.h>
class vgui_window;

class segv_vil_segmentation_manager : public vgui_wrapper_tableau
{
 public:
  segv_vil_segmentation_manager();
  ~segv_vil_segmentation_manager();
  static segv_vil_segmentation_manager *instance();
  void quit();
  void load_image();
  void save_image();
  void set_range_params();
  void threshold_image();
  void harris_corners();
  void vd_edges();
  void fit_lines();
  void fit_conics();
  void regions();
  //: visualization
  void display_images_as_color();
  //: image arithmetic
  void add_images();
  void subtract_images();

  //: tests and development only
    void test_inline_viewer();
    void test_ellipse_draw();
  //:internals
  void clear_display();
  void init();

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

 protected:
  void draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges, bool verts=false);
  void draw_lines(vcl_vector<vsol_line_2d_sptr> const & line_segs,
                  const vgui_style_sptr& style = NULL);

  void draw_conics(vcl_vector<vsol_conic_2d_sptr> const & conic_segs,
                  const vgui_style_sptr& style = NULL);

  void draw_polylines(vcl_vector<vsol_polyline_2d_sptr> const & polys);
  void draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
                    bool verts=false);
  void draw_points(vcl_vector<vsol_point_2d_sptr> const & points,
                   const vgui_style_sptr& style);

  void set_selected_grid_image(vil_image_resource_sptr const& image,
                               vgui_range_map_params_sptr const& rmps =0);
                               
  void add_image_at(vil_image_resource_sptr const& image,
                    const unsigned col, const unsigned row,
                    vgui_range_map_params_sptr const& rmps = 0);
                    
  void add_image(vil_image_resource_sptr const& image,
                 vgui_range_map_params_sptr const& rmps =0);
                 
  vil_image_resource_sptr selected_image();
  vil_image_resource_sptr image_at(const unsigned col, const unsigned row);
  bool
  set_image_at(const unsigned col, const unsigned row, vil_image_resource_sptr const& image);
  bgui_image_tableau_sptr selected_image_tab();
  bgui_vtol2D_tableau_sptr selected_vtol2D_tab();
  bgui_picker_tableau_sptr selected_picker_tab();
  vgui_rubberband_tableau_sptr selected_rubber_tab();
  vtol_face_2d_sptr face_at(const int col, const int row);
  vgui_range_map_params_sptr range_params(vil_image_resource_sptr const& image);
 private:
  //flags
  bool first_; //first image load
  vgui_window* win_;
  vgui_grid_tableau_sptr grid_;
  vtol_face_2d_sptr foreground_face_;
  vtol_face_2d_sptr background_face_;
  bgui_bargraph_clipon_tableau_sptr bargraph_;
  static segv_vil_segmentation_manager *instance_;
};

#endif // segv_vil_segmentation_manager_h_
