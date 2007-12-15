#ifndef bwm_observer_cam_rat_h_
#define bwm_observer_cam_rat_h_
//:
// \file

#include "bwm_observer_cam.h"

#include <vcl_iosfwd.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vpgl/vpgl_rational_camera.h>

#include <bgeo/bgeo_lvcs.h>

//#include <bmsh3d/bmsh3d_vertex.h>

class bwm_observer_cam_rat : public bwm_observer_cam
{
 public:

  bwm_observer_cam_rat() { lvcs_ =0; }
    //: proj_plane_(vgl_homg_plane_3d<double>(0, 0, 1, 0)) {}

  bwm_observer_cam_rat(bgui_image_tableau_sptr const& img,
    vpgl_rational_camera<double> *camera,
    vcl_string cam_path)
    : bwm_observer_cam(img, camera, cam_path), lvcs_(0) { }

  // set the initial projection plane to z=0
  bwm_observer_cam_rat(bgui_image_tableau_sptr const& img, const char* n="unnamed")
    : bwm_observer_cam(img, n), lvcs_(0) {}

  virtual ~bwm_observer_cam_rat(){};

  virtual vcl_string type_name() const { return "bwm_observer_cam_rat"; }

  void set_camera(vpgl_rational_camera<double> *camera, vcl_string cam_path)
  { camera_ = camera; bwm_observer_cam::set_camera(camera, cam_path);}

  virtual bool intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
                                       vgl_homg_plane_3d<double> plane,
                                       vgl_homg_point_3d<double> &world_point);

  bool find_intersection_point(vgl_homg_point_2d<double> img_point,
                               vsol_polygon_3d_sptr poly3d,
                               vgl_point_3d<double>& point3d);

  void camera_center(vgl_homg_point_3d<double> &center);

  //vgl_vector_3d<double> camera_direction(vgl_point_3d<double> origin);
  vgl_vector_3d<double> camera_direction();

  vcl_ostream& print_camera(vcl_ostream& s);

  bool shift_camera(double dx, double dy);
  void adjust_camera_offset(vsol_point_2d_sptr img_point);
  void center_pos();
  void print_selected_vertex();
  void set_lvcs_at_selected_vertex();
  void save_lvcs();
  void convert_file_to_lvcs();
  void load_lvcs();
  void define_lvcs(float x1, float y1);

  //*********** Save methods
  void save_selected();
  void save_all();
  void save_gml();
  void save_kml();
  void save_kml_collada();
  void save_x3d();

  void generate_textures();

 protected:
  bgeo_lvcs* lvcs_;
  vcl_string select_file();
  void set_selected_vertex();
};

#endif
