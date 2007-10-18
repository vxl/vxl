#include "bwm_observer_cam_proj.h"

#include <vgl/algo/vgl_homg_operators_3d.h>

bool bwm_observer_cam_proj::intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
                                                    vgl_homg_plane_3d<double> plane,
                                                    vgl_homg_point_3d<double> &world_point)
{ 
  vgl_homg_line_3d_2_points<double> ray = camera_->backproject(img_point);
  vgl_homg_operators_3d<double> oper;
  vgl_homg_point_3d<double> p = oper.intersect_line_and_plane(ray,plane);
  world_point = p;
  return true;
}

vgl_vector_3d<double> bwm_observer_cam_proj::camera_direction(vgl_point_3d<double> origin)
{
  vgl_vector_3d<double> direction(0.0, 0.0 ,0.0);

  //cam_center.set(camera_->camera_center().x(),camera_->camera_center().y(),camera_->camera_center().z());
  vgl_point_3d<double> cam_center(camera_->camera_center());
  direction.set(cam_center.x() - origin.x(), cam_center.y() - origin.y(), cam_center.z() - origin.z());
  direction = direction / direction.length();

  return direction;
}

vcl_ostream& bwm_observer_cam_proj::print_camera(vcl_ostream& s)
{
  s << *(this->camera_);
  return s;
}
