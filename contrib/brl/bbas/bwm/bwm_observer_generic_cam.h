#ifndef bwm_observer_generic_cam_h_
#define bwm_observer_generic_cam_h_

#include "bwm_observer_cam.h"

#include <vcl_iosfwd.h>

#include <vgl/vgl_fwd.h>

#include <vpgl/vpgl_generic_camera.h>

#include <bmsh3d/bmsh3d_vertex.h>

#include <vil/vil_image_resource.h>

class bwm_observer_generic_cam : public bwm_observer_cam
{
 public:

  bwm_observer_generic_cam(bgui_image_tableau_sptr img,
                           vcl_string name,
                           vcl_string& image_path,
                           vcl_string& cam_path,
                           vcl_string& sub_type,
                           bool display_image_path);

  // set the initial projection plane to z=0
  bwm_observer_generic_cam(bgui_image_tableau_sptr img, const char* n="unnamed")
    : bwm_observer_cam(img, n) {}

  virtual ~bwm_observer_generic_cam(){}

  virtual vcl_string type_name() const { return "bwm_observer_generic_cam"; }

  void set_camera(vpgl_generic_camera<double> *camera, vcl_string cam_path)
  { bwm_observer_cam::set_camera(camera, cam_path);}

  virtual bool intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                       vgl_plane_3d<double> plane,
                                       vgl_point_3d<double> &world_point);
  
  vil_image_resource_sptr ray_image(int component, int level) const;


 protected:
};

#endif // bwm_observer_generic_cam_h_
