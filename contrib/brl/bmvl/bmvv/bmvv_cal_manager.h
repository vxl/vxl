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
// Currently works on a single view. Provides interactive tools for
// corresponding 3-d points to 2-d image locations. Also provides for
// the solution of planar camera models (homographies).  The homography can be
// extended to a vgl_p_matrix by selecting a number of points off the
// ground plane. In addition there are several simple 3-d modeling tools:
//      o Backproject a polygon onto a X-Y world plane (specified Z)
//      o Create a 3-d box from three points on the roof face (specified Z)
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
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_image_tableau_sptr.h>
#include <vgui/vgui_range_map_params_sptr.h>
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

  //: methods for menu callbacks
  void quit();
  void load_image();
  void save_image();

  //: 3-d points and polygons representing a world model
  void load_world();

  //: viewing functions
  void adjust_contrast();
  void set_range_params();
  void clear_display();
  void clear_selected();
  void load_image_and_camera();

  //: pairs of corresponding 2-d image and 3-d world points
  void load_corrs();
  void save_corrs();
  void pick_correspondence();
  void remove_correspondence();

  //: world-to-image correspondences and vertical lines for constrained cameras
  void pick_vertical();
  void save_constraints();

  //: camera modeling
  void set_identity_camera();
  void solve_camera();
  void compute_ransac_homography();
  void save_camera();

  //: backproject an image onto the world plane
  void map_image_to_world();

  // == modeling functions ==

  //: set the create XY plane height (Z)
  void set_height();
  //: define a polygon on the create plane
  void pick_polygon();
  //: clear all current world information
  void clear_world();
  //: insert a created polygon into the world data structures
  void confirm_polygon();
  //: save the world information in cal format
  void save_world();
  //: save the world information in ply2 format (a single indexed face set)
  void save_world_ply2();
  //: read the world information in ply2 format (a single indexed face set)
  void read_world_ply2();
  //: Convert boxes in a merged indexed face set to individual boxes in VRML
  // (A temporary tool to unmerge files consisting of all boxes)
  void ply2_to_vrml();
  //: Create a box in the world from three top face corner points on the create plane.  The bottom plane is at Z=0.
  void create_box();
  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}
  void init();

  //: the virtual handle function
  virtual bool handle(const vgui_event&);

  void load_image_file(vcl_string image_filename, bool greyscale);
 protected:
  //: internal utility methods
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

void draw_vsol_polygon_3d(vsol_polygon_3d_sptr const & poly,
                          bool clear, const float r=0, const float g=1,
                          const float b=0);

  void draw_polygons_3d(bool clear, const float r,
                        const float g, const float b);

  void add_image(vil_image_resource_sptr const& image,
               vgui_range_map_params_sptr rmps = 0,
               bool greyscale =false);
  vil1_image get_image_at(unsigned col, unsigned row);
  bool read_corrs(vcl_ifstream& str);
  bool draw_correspondences();
  bool draw_projected_world_points();
  void add_poly_vertices_to_world_pts(vsol_polygon_3d_sptr const& poly);
  void clear_correspondences();
  void project_world();
  int  get_selected_proj_world_pt(vgl_point_2d<double>&  pt);
  vgui_range_map_params_sptr
    range_params(vil_image_resource_sptr const& image);
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
  //: the current camera
  vgl_p_matrix<double> cam_;
  //: the previous camera before the current solution
  vgl_p_matrix<double> prev_cam_;
  //: lines in the image corresponding to the vertical direction
  vcl_vector<vgl_line_segment_2d<double> > verticals_;
  //: the Z height of the create plane
  double height_;
  //: the polygon just created
  vsol_polygon_3d_sptr new_poly_;
  //: all the polygons in the world model
  vcl_vector<vsol_polygon_3d_sptr> polys_;
  //: the 2-d projected form of the polygons
  vcl_vector<vsol_polygon_2d_sptr> proj_polys_;
  //: an indexed face set for all the polygons in the world
  vcl_vector<vcl_vector<unsigned> > indexed_face_set_;
};

#endif // bmvv_cal_manager_h_
