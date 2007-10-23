#include "bwm_observer_cam_proj.h"

#include <vgl/algo/vgl_homg_operators_3d.h>

bool bwm_observer_cam_proj::intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
                                                    vgl_homg_plane_3d<double> plane,
                                                    vgl_homg_point_3d<double> &world_point)
{ 
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  vgl_homg_line_3d_2_points<double> ray = proj_cam->backproject(img_point);
  vgl_homg_operators_3d<double> oper;
  vgl_homg_point_3d<double> p = oper.intersect_line_and_plane(ray,plane);
  world_point = p;
  return true;
}

void bwm_observer_cam_proj::camera_center(vgl_homg_point_3d<double> &center) 
{ 
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  center = proj_cam->camera_center(); 
}

vgl_vector_3d<double> bwm_observer_cam_proj::camera_direction(vgl_point_3d<double> origin)
{
  vgl_vector_3d<double> direction(0.0, 0.0 ,0.0);
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  //cam_center.set(camera_->camera_center().x(),camera_->camera_center().y(),camera_->camera_center().z());
  vgl_point_3d<double> cam_center(proj_cam->camera_center());
  direction.set(cam_center.x() - origin.x(), cam_center.y() - origin.y(), cam_center.z() - origin.z());
  direction = direction / direction.length();

  return direction;
}

vcl_ostream& bwm_observer_cam_proj::print_camera(vcl_ostream& s)
{
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  s << *proj_cam;
  return s;
}
