// This is brl/bmvl/bmvv/bmvv_cal_manager.h
#ifndef bmvv_cal_manager_h_
#define bmvv_cal_manager_h_
//----------------------------------------------------------------------------
//:
// \file
// \brief Manager for camera calibration
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy Jan 15, 2005    Initial version.
// \endverbatim
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_fstream.h>
//#include <vil1/vil1_image.h>
#include <vil/vil_image_resource_sptr.h>
#include <vgl/algo/vgl_p_matrix.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vtol/vtol_intensity_face_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <vgui/vgui_style_sptr.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_soview2D.h>
#include <bgui/bgui_vtol2D_tableau_sptr.h>
#include <bgui/bgui_picker_tableau_sptr.h>

class vgui_window;
class vgui_soview;
//: A manager for displaying segmentation results.
class bmvv_cal_manager : public vgui_wrapper_tableau
{
 public:
  bmvv_cal_manager();
 ~bmvv_cal_manager();
  static bmvv_cal_manager *instance();

  //:methods for menu callbacks
  void quit();
  void load_image();
  void adjust_contrast();
  void clear_display();
  void clear_selected();
  void compute_ransac_homography();
  void load_image_and_camera();
  void load_corrs();
  void save_corrs();
  void load_world();
  void pick_correspondence();
  void remove_correspondence();
  void solve_camera();
  void save_camera();

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();

  //: the virtual handle function
  virtual bool handle(const vgui_event&);

  void load_image_file(vcl_string image_filename, bool greyscale);
 protected:
  //:internal utility methods
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

  void add_image(vil_image_resource_sptr const& image, bool greyscale=false);
  vil1_image get_image_at(unsigned col, unsigned row);
  bool read_corrs(vcl_ifstream& str);
  bool draw_correspondences();
  bool draw_projected_world_points();
  void clear_correspondences();
  void project_world();
  int  get_selected_proj_world_pt(vgl_point_2d<double>&  pt);
  void draw_line(float x0, float y0, float x1, float y1);
 private:
  vil_image_resource_sptr img_;
  vgui_window* win_;
  bgui_image_tableau_sptr itab_;
  bgui_picker_tableau_sptr ptab_;
  bgui_vtol2D_tableau_sptr btab_;
  vcl_map<int, int> point_3d_map_;
  static bmvv_cal_manager *instance_;
  vcl_vector<vgl_point_2d<double> > corrs_;
  vcl_vector<bool>  corrs_valid_;
  vcl_vector<vgl_point_2d<double> >  proj_image_pts_;
  vcl_vector<vgui_soview2D_point* >  corr_sovs_;
  vcl_vector<vgl_point_3d<double> > world_;
  vgl_p_matrix<double> cam_;
  vgl_p_matrix<double> prev_cam_;
};

#endif // bmvv_cal_manager_h_
