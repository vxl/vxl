//--*-c++-*--
// This is brl/bmvl/bmvv/bmvv_recon_manager.h
#ifndef bmvv_recon_manager_h_
#define bmvv_recon_manager_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Manager for 3-d reconstruction 
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy February 07, 2004    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vil1/vil1_image.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_easy2D_tableau_sptr.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_grid_tableau_sptr.h>
#include <mvl/FMatrix.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>
#include <brct/brct_corr_sptr.h>
#include <brct/brct_plane_calibrator.h>
#include <brct/brct_plane_sweeper.h>
#include <brct/brct_volume_processor.h>

class vgui_window;
class vgui_soview;
//: A manager for displaying segmentation results.
class bmvv_recon_manager : public vgui_wrapper_tableau
{
 public:
  bmvv_recon_manager();
  ~bmvv_recon_manager();
  static bmvv_recon_manager *instance();

  //:methods for menu callbacks
  void quit();
  void load_image();
  void clear_display();
  void clear_selected();
  void read_3d_points();
  void initial_model_projection();
  void model_projection();
  void print_selected_corr();
  void pick_corr();
  void write_corrs();
  void read_corrs();
  void compute_homographies();
  void write_homographies();
  void read_homographies();
  void project_image();
  void map_image();
  void set_images();
  void compute_harris_corners();
  void overlapping_projections();
  void overlapping_projections_z();
  void overlapping_harris_proj_z();
  void cross_correlate_plane();
  void cross_correlate_z();
  void cross_correlate_harris_z();
  void depth_image();
  void harris_depth_match();
  void z_corr_image();
  void corr_plot();
  void map_point();
  void map_harris_corners();
  void match_harris_corners();
  void harris_sweep();
  void display_matched_corners();
  void display_harris_3d();
  void write_points_vrml();
  void read_points_vrml();
  void write_volumes_vrml();
  void read_change_data();
  void write_change_volumes_vrml();
  void compute_change();
  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();
  //: the virtual handle function
  virtual bool handle(const vgui_event&);
  void load_image_file(vcl_string image_filename, bool greyscale, unsigned col, unsigned row);

 protected:
  //:internal utility methods
  void draw_regions(vcl_vector<vtol_intensity_face_sptr>& regions,
                    bool verts=false);
  void create_point(int& cam, vsol_point_2d_sptr& p);
  int get_cam();
  brct_corr_sptr get_selected_corr();
  void draw_corr_point(const float x, const float y);
  void draw_vsol_points(const int cam,
                        vcl_vector<vsol_point_2d_sptr> const & points,
                        bool clear = true,
                        const float r = 0,
                        const float g = 1,
                        const float b = 0);

  void draw_vsol_point(const int cam,
                       vsol_point_2d_sptr const & point,
                       bool clear = false,
                       const float r = 0,
                       const float g = 1,
                       const float b = 0);

void draw_vsol_3d_points(const int cam,
                         vcl_vector<vsol_point_3d_sptr> const& pts3d,
                         bool clear = true);

  bgui_vtol2D_tableau_sptr get_vtol2D_tableau_at(unsigned col, unsigned row);
  bgui_vtol2D_tableau_sptr get_selected_vtol2D_tableau();
  bgui_picker_tableau_sptr get_picker_tableau_at(unsigned col, unsigned row);
  bgui_picker_tableau_sptr get_selected_picker_tableau();
  vil1_image get_image_at(unsigned col, unsigned row);

  //
 private:
  bool images_set_;
  bool harris_set_;
  vil1_image img_;
  vgui_window* win_;
  vcl_vector<bgui_vtol2D_tableau_sptr> vtol_tabs_;
  vgui_grid_tableau_sptr grid_;
  brct_plane_calibrator cal_;
  brct_plane_sweeper sweep_;
  brct_volume_processor vproc_;
  int plane_;
  vcl_map<int, int> point_3d_map_;
  static bmvv_recon_manager *instance_;
};

#endif // bmvv_recon_manager_h_
