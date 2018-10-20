#include "bwm_observer_generic_cam.h"
//
#include "bwm_observer_mgr.h"
#include "bwm_observer_proj_cam.h"
#include "algo/bwm_utils.h"
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_ray_3d.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#define DEBUG

bwm_observer_generic_cam::bwm_observer_generic_cam(bgui_image_tableau_sptr img,
                                                   std::string name,
                                                   std::string& image_path,
                                                   std::string& cam_path,
                                                   std::string& subtype,
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
  int ni = img_res->ni(), nj = img_res->nj();
  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a separate file
  if (cam_path.size() == 0)
  {
    bwm_utils::show_error("Camera tableaus need a valid camera path!");
    return;
  }
  this->set_camera_path(cam_path);
  bool local = false;
  vpgl_camera<double>* cam =
    bwm_observer_proj_cam::read_camera(cam_path, "perspective", ni, nj);
  if (!cam)
    cam = bwm_observer_proj_cam::read_camera(cam_path, "projective", ni, nj);
  if (!cam)
    cam = bwm_observer_rat_cam::read_camera(cam_path, local);
  if (!cam||!local)
    camera_ = nullptr;
  else {
    vpgl_generic_camera<double> gcam;
    vpgl_generic_camera_convert::convert(cam, ni, nj, gcam);
    camera_ = new vpgl_generic_camera<double>(gcam);
  }

  //generate a unique tab name if null
  if (name=="")
    name = cam_path;
  set_tab_name(name);
  // add the observer to the observer pool
  bwm_observer_mgr::instance()->add(this);
}

bool bwm_observer_generic_cam::intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                                       vgl_plane_3d<double> plane,
                                                       vgl_point_3d<double> &world_point)
{
  vpgl_generic_camera<double>* generic_cam = static_cast<vpgl_generic_camera<double> *> (camera_);
  double ni = generic_cam->cols(), nj = generic_cam->rows();
  if (img_point.x()<0.0 ||img_point.y()<0.0||
      img_point.x()>=ni||img_point.y()>=nj)
    return false;
  vgl_ray_3d<double> ray = generic_cam->ray(img_point.x(), img_point.y());
  return vgl_intersection(ray, plane, world_point);
}


vil_image_resource_sptr
bwm_observer_generic_cam::ray_image(int component, int level=0) const
{
  bool orgt = (component == 0);
  bool dirt = (component == 1);
  vpgl_generic_camera<double>* gcam =
    static_cast<vpgl_generic_camera<double> *> (camera_);
  if (!gcam) return nullptr;
  vbl_array_2d<vgl_ray_3d<double> > rays = gcam->rays(level);
  int nc = rays.cols(), nr = rays.rows();

  vil_image_view<float> view(nc, nr, 3);
  for (int r = 0; r<nr; ++r)
    for (int c = 0; c<nc; ++c) {
      if (orgt) {
        vgl_point_3d<double> org = rays[r][c].origin();
        view(c,r,0) = static_cast<float>(org.x());
        view(c,r,1) = static_cast<float>(org.y());
        view(c,r,2) = static_cast<float>(org.z());
      }
      else if (dirt) {
        vgl_vector_3d<double> dir = rays[r][c].direction();
        view(c,r,0) = static_cast<float>(dir.x());
        view(c,r,1) = static_cast<float>(dir.y());
        view(c,r,2) = static_cast<float>(dir.z());
      }
      else return nullptr;
    }
  return vil_new_image_resource_of_view(view);
}
