#ifndef bmvv_f_manager_h_
#define bmvv_f_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Manager for multiview geometry display
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy February 19, 2007    Initial version.
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
#include <vgui/vgui_style.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_style_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <bgui/bgui_bargraph_clipon_tableau_sptr.h>
#include <bgui/bgui_graph_tableau_sptr.h>
#include <bgui/bgui_graph_tableau.h>
#include <brct/brct_corr_sptr.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vpgl/vpgl_proj_camera.h>
class vgui_window;

class bmvv_f_manager : public vgui_wrapper_tableau
{
 public:
  bmvv_f_manager();
  ~bmvv_f_manager();
  static bmvv_f_manager *instance();
  void quit();
  void load_image();
  void save_image();

  //: visualization
  void set_range_params();
  void intensity_profile();
  void intensity_histogram();
  
 //:utilities
  void clear_display();
  void clear_all();
  void init();
#if 0
  void remove_image();
  void convert_to_grey();
#endif
  //:debug and development routines
  void load_image_and_cam();
  void read_corrs();
  void save_corrs();
  void load_world();
  void save_world();
  void display_corrs();
  void read_f_matrix();
  void save_f_matrix();
  void display_left_epi_lines();
  void display_right_epi_lines();
  void display_picked_epi_line();
  void create_correspondence();
  void pick_correspondence();
  void compute_f_matrix();
  void reconstruct_world();
  void project_world();
  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

 protected: 
  void draw_edges(vcl_vector<vtol_edge_2d_sptr>& edges, bool verts=false);
  void draw_lines(vcl_vector<vsol_line_2d_sptr> const & line_segs,
                  const vgui_style_sptr& style = NULL);

  void draw_conics(vcl_vector<vsol_conic_2d_sptr> const & conic_segs,
                  const vgui_style_sptr& style = NULL);

  void draw_polylines(vcl_vector<vsol_polyline_2d_sptr> const & polys,
                      vgui_style_sptr style=(vgui_style*)0);
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
  bgui_vtol2D_tableau_sptr vtol2D_tab_at(const unsigned col,const unsigned row);
  bgui_vtol2D_tableau_sptr selected_vtol2D_tab();
  bgui_picker_tableau_sptr selected_picker_tab();
  vtol_face_2d_sptr face_at(const int col, const int row);
  vgui_range_map_params_sptr range_params(vil_image_resource_sptr const& image);
  brct_corr_sptr get_selected_corr();
  bool point_lists(vcl_vector<vgl_point_2d<double> >& lpts,
                   vcl_vector<vgl_point_2d<double> >& rpts);

 private:
  //flags
  bool first_; //first image load
  vgui_window* win_;
  vgui_grid_tableau_sptr grid_;
  vtol_face_2d_sptr foreground_face_;
  vtol_face_2d_sptr background_face_;
  bgui_bargraph_clipon_tableau_sptr bargraph_;
  vcl_vector<brct_corr_sptr> corrs_;
  vpgl_fundamental_matrix<double> fm_;
  vcl_map<int, int> corr_map_;
  vcl_vector<vgl_point_3d<double> > world_;
  vcl_vector<bgui_vtol2D_tableau_sptr> vtabs_;
  vcl_map<int, vpgl_proj_camera<double>*> cam_map_;
  static bmvv_f_manager *instance_;
};

#endif // bmvv_f_manager_h_
