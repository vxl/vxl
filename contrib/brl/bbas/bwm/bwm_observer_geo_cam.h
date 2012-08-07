#ifndef bwm_observer_geo_cam_h_
#define bwm_observer_geo_cam_h_

#include "bwm_observer_cam.h"

#include <vcl_iosfwd.h>

#include <vgl/vgl_fwd.h>

#include <vpgl/file_formats/vpgl_geo_camera.h>

#include <bmsh3d/bmsh3d_vertex.h>

class bwm_observer_geo_cam : public bwm_observer_cam
{
 public:

  bwm_observer_geo_cam(bgui_image_tableau_sptr img,
                       vcl_string name,
                       vcl_string& image_path,
                       vcl_string& cam_path,
                       bool display_image_path);

  // set the initial projection plane to z=0
  bwm_observer_geo_cam(bgui_image_tableau_sptr img, const char* n="unnamed")
    : bwm_observer_cam(img, n) {}

  virtual ~bwm_observer_geo_cam(){}

  virtual vcl_string type_name() const { return "bwm_observer_geo_cam"; }

  void set_camera(vpgl_geo_camera *camera, vcl_string cam_path)
  { bwm_observer_cam::set_camera(camera, cam_path);}


  virtual bool intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                       vgl_plane_3d<double> plane,
                                       vgl_point_3d<double> &world_point);

  vcl_ostream& print_camera(vcl_ostream& s);

  static vpgl_camera<double>* 
    read_camera(vcl_string cam_path);

 protected:
  vcl_string subtype_;

};

#endif // bwm_observer_geo_cam_h_
