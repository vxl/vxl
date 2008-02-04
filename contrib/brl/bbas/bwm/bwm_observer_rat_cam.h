#ifndef bwm_observer_rat_cam_h_
#define bwm_observer_rat_cam_h_
//:
// \file
#include "bwm_observer_cam.h"

#include <vcl_iosfwd.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_3d_sptr.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/bgeo/bgeo_lvcs.h>

#include <bgui/bgui_image_tableau_sptr.h>

class bwm_observer_rat_cam : public bwm_observer_cam
{
 public:

  typedef bwm_observer_cam base;
#if 0
  bwm_observer_rat_cam() {}
  bwm_observer_cam(img, camera, cam_path) { }
#endif
  bwm_observer_rat_cam(bgui_image_tableau_sptr img,
    vcl_string& name,
    vcl_string& image_path,
    vcl_string& cam_path,
    bool display_image_path);

  // set the initial projection plane to z=0
  bwm_observer_rat_cam(bgui_image_tableau_sptr img, const char* n="unnamed")
    : bwm_observer_cam(img, n) {}

  virtual ~bwm_observer_rat_cam(){};

  virtual vcl_string type_name() const { return "bwm_observer_rat_cam"; }

  bool handle(const vgui_event &e) { return base::handle(e); }

  void set_camera(vpgl_rational_camera<double> *camera, vcl_string cam_path)
  { camera_ = camera; bwm_observer_cam::set_camera(camera, cam_path);}

  vpgl_rational_camera<double> camera();

  virtual bool intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                       vgl_plane_3d<double> plane,
                                       vgl_point_3d<double> &world_point);

  bool find_intersection_point(vgl_point_2d<double> img_point,
                               vsol_polygon_3d_sptr poly3d,
                               vgl_point_3d<double>& point3d);

  void camera_center(vgl_point_3d<double> &center);

  //vgl_vector_3d<double> camera_direction(vgl_point_3d<double> origin);
  vgl_vector_3d<double> camera_direction();

  vcl_ostream& print_camera(vcl_ostream& s);

  bool shift_camera(double dx, double dy);
  void adjust_camera_offset(vsol_point_2d_sptr img_point);
  void center_pos();
  void print_selected_vertex();
  void set_lvcs_at_selected_vertex();
  void define_lvcs(float x1, float y1);

  // *********** Save methods ******************

  void save_selected();
  void save_all();
  void save_gml();
  void save_kml();
  void save_kml_collada();
  void save_x3d();

  void generate_textures();

  // given that there exists at least one world_to_image correspondence
  // in the site align the camera of this observer to that world point
  void adjust_camera_to_world_pt();

  // project edges from the master to this image using the
  // master's projection plane to transfer. Just a temporary experiment
  // to understand how edges behave across image resolution and sensor
  // modality. **Not to be used in production**
  void project_edges_from_master();

  // project edges from the master to a target search image and register
  void register_search_to_master();

 protected:
  vcl_string select_file();
  void set_selected_vertex();
};

#endif
