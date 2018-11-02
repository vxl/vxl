#ifndef bwm_observer_geo_cam_h_
#define bwm_observer_geo_cam_h_

#include <iostream>
#include <iosfwd>
#include "bwm_observer_cam.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_fwd.h>

#include <vpgl/file_formats/vpgl_geo_camera.h>

#include <bmsh3d/bmsh3d_vertex.h>

class bwm_observer_geo_cam : public bwm_observer_cam
{
 public:

  bwm_observer_geo_cam(bgui_image_tableau_sptr img,
                       std::string name,
                       std::string& image_path,
                       std::string& cam_path,
                       bool display_image_path);

  // set the initial projection plane to z=0
  bwm_observer_geo_cam(bgui_image_tableau_sptr img, const char* n="unnamed")
    : bwm_observer_cam(img, n) {}

  virtual ~bwm_observer_geo_cam(){}

  virtual std::string type_name() const { return "bwm_observer_geo_cam"; }

  void set_camera(vpgl_geo_camera *camera, std::string cam_path)
  { bwm_observer_cam::set_camera(camera, cam_path);}


  virtual bool intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                       vgl_plane_3d<double> plane,
                                       vgl_point_3d<double> &world_point);

  std::ostream& print_camera(std::ostream& s);

  static vpgl_camera<double>*
    read_camera(std::string cam_path);

 protected:
  std::string subtype_;

};

#endif // bwm_observer_geo_cam_h_
