#ifndef bwm_observer_proj_cam_h_
#define bwm_observer_proj_cam_h_

#include "bwm_observer_cam.h"

#include <vcl_iosfwd.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

#include <vpgl/vpgl_proj_camera.h>

#include <bmsh3d/bmsh3d_vertex.h>

class bwm_observer_proj_cam : public bwm_observer_cam
{
 public:

  bwm_observer_proj_cam(bgui_image_tableau_sptr img,
    vcl_string name, 
    vcl_string& image_path, 
    vcl_string& cam_path,                
    bool display_image_path); 

  // set the initial projection plane to z=0
  bwm_observer_proj_cam(bgui_image_tableau_sptr img, const char* n="unnamed")
    : bwm_observer_cam(img, n) {}

  virtual ~bwm_observer_proj_cam(){}

  virtual vcl_string type_name() const { return "bwm_observer_proj_cam"; }

  void set_camera(vpgl_proj_camera<double> *camera, vcl_string cam_path)
  { bwm_observer_cam::set_camera(camera, cam_path);}

  void camera_center(vgl_homg_point_3d<double> &center);

  vgl_vector_3d<double> camera_direction(vgl_point_3d<double> origin);

  virtual bool intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
                                       vgl_homg_plane_3d<double> plane,
                                       vgl_homg_point_3d<double> &world_point);

  vpgl_proj_camera<double> read_projective_camera(vcl_string cam_path);

  vcl_ostream& print_camera(vcl_ostream& s);
};

#endif
