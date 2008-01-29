#include "bwm_observer_proj_cam.h"
#include "bwm_observer_mgr.h"
#include "algo/bwm_utils.h"

#include <vgl/algo/vgl_homg_operators_3d.h>

bwm_observer_proj_cam::bwm_observer_proj_cam(bgui_image_tableau_sptr img,
    vcl_string name, 
    vcl_string& image_path, 
    vcl_string& cam_path,                
    bool display_image_path)
: bwm_observer_cam(img)
{

  img->show_image_path(display_image_path);

  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = bwm_utils::load_image(image_path, params);

  if (!img_res) {
     bwm_utils::show_error("Image [" + image_path + "] is NOT found");
     return;
  }

  img->set_image_resource(img_res, params);
  img->set_file_name(image_path);

  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a seperate file
  if (cam_path.size() == 0)
  {
    bwm_utils::show_error("Camera tableaus need a valid camera path!");
    return;
  } 
  camera_ = read_projective_camera(cam_path).clone();
  
  // add the observer to the observer pool
  bwm_observer_mgr::instance()->add(this);
  set_tab_name(name);
}

bool bwm_observer_proj_cam::intersect_ray_and_plane(vgl_homg_point_2d<double> img_point,
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

void bwm_observer_proj_cam::camera_center(vgl_homg_point_3d<double> &center) 
{ 
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  center = proj_cam->camera_center(); 
}

vgl_vector_3d<double> bwm_observer_proj_cam::camera_direction(vgl_point_3d<double> origin)
{
  vgl_vector_3d<double> direction(0.0, 0.0 ,0.0);
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  //cam_center.set(camera_->camera_center().x(),camera_->camera_center().y(),camera_->camera_center().z());
  vgl_point_3d<double> cam_center(proj_cam->camera_center());
  direction.set(cam_center.x() - origin.x(), cam_center.y() - origin.y(), cam_center.z() - origin.z());
  direction = direction / direction.length();

  return direction;
}

vpgl_proj_camera<double>
bwm_observer_proj_cam::read_projective_camera(vcl_string cam_path)
{
  vcl_ifstream cam_stream(cam_path.data());
  vpgl_proj_camera<double> cam;
  cam_stream >> cam;
#ifdef DEBUG
  vcl_cout << cam << vcl_endl;
#endif

  return cam;
}

vcl_ostream& bwm_observer_proj_cam::print_camera(vcl_ostream& s)
{
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  s << *proj_cam;
  return s;
}
