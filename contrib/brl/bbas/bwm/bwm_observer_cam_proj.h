#ifndef bwm_observer_cam_proj_h_
#define bwm_observer_cam_proj_h_

#include "bwm_observer_cam.h"

#include <vcl_iosfwd.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_3d_sptr.h>

#include <vpgl/vpgl_proj_camera.h>

#include <bmsh3d/bmsh3d_vertex.h>

class bwm_observer_cam_proj : public bwm_observer_cam
{
 public:

  bwm_observer_cam_proj(){}

  bwm_observer_cam_proj(bgui_image_tableau_sptr const& img,
    vpgl_proj_camera<double> *camera, vcl_string cam_path)
    : bwm_observer_cam(img, camera, cam_path) {}

  // set the initial projection plane to z=0
  bwm_observer_cam_proj(bgui_image_tableau_sptr const& img, const char* n="unnamed")
    : bwm_observer_cam(img, n) {}

  virtual ~bwm_observer_cam_proj(){}

  virtual vcl_string type_name() const { return "bwm_observer_cam_proj"; }

  void set_camera(vpgl_proj_camera<double> *camera, vcl_string cam_path)
  { bwm_observer_cam::set_camera(camera, cam_path);}

  void camera_center(vgl_homg_point_3d<double> &center);

  vgl_vector_3d<double> camera_direction(vgl_point_3d<double> origin);

  virtual bool intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
                                       vgl_homg_plane_3d<double> plane,
                                       vgl_homg_point_3d<double> &world_point);

  vcl_ostream& print_camera(vcl_ostream& s);
};

#endif
