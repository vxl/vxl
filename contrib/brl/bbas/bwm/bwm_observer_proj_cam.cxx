#include "bwm_observer_proj_cam.h"
#include "bwm_observer_mgr.h"
#include "algo/bwm_utils.h"
#include <vul/vul_file.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_proj_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <bwm/io/bwm_io_kml_camera.h>
#include <bpgl/bpgl_camera_utils.h>
#define DEBUG

vpgl_camera<double>* bwm_observer_proj_cam::
read_camera(std::string cam_path, std::string subtype,
            unsigned ni, unsigned nj)
{
  std::string ext = vul_file_extension(cam_path);
  if (ext == ".rpc")
    return nullptr;
  if (subtype=="perspective") {
    if (ext == ".vsl") // binary form
    {
      vpgl_perspective_camera<double> pcam;
      vsl_b_ifstream bp_in(cam_path.c_str());
      if (!bp_in) {
        std::cerr << "In bwm_observer_proj_cam::read_camera(.) -\n"
                 << " invalid binary camera file " << cam_path.data() << '\n';
        return nullptr;
      }
      vsl_b_read(bp_in, pcam);
      bp_in.close();
      vpgl_proj_camera<double> cam(pcam.get_matrix());
      return cam.clone();
    }
    else if (ext == ".kml") {
      double right_fov, top_fov;
      double altitude, heading;
      double tilt, roll;
      bool success =
        bwm_io_kml_camera::read_camera(cam_path, right_fov, top_fov,
                                       altitude, heading, tilt, roll);
      if (!success) {
        std::cerr << "In bwm_observer_proj_cam::read_kml_camera(.) -\n"
                 << " invalid binary camera file " << cam_path.data() << '\n';
        return nullptr;
      }
      vpgl_perspective_camera<double> cam =
        bpgl_camera_utils::camera_from_kml(ni, nj, right_fov, top_fov,
                                           altitude, heading, tilt, roll);
      return new vpgl_perspective_camera<double>(cam);
    }
    //An ASCII stream for perspective camera
    vpgl_perspective_camera<double> pcam;
    std::ifstream cam_stream(cam_path.data());

    if (!cam_stream.is_open())
    {
      std::cerr << "In bwm_observer_proj_cam::read_projective_camera(.) -\n"
               << " invalid camera file " << cam_path.data() << '\n';
      return nullptr;
    }

    cam_stream >> pcam;
#ifdef DEBUG
    std::cout << pcam << std::endl;
#endif
#if 0 // was returning a projective cam
    vpgl_proj_camera<double> cam(pcam.get_matrix());
    return cam.clone();
#endif
    // now returns a perspective cam
    return new vpgl_perspective_camera<double>(pcam);
  }
  //must be an ASCII stream for projective camera
  std::ifstream cam_stream(cam_path.data());
  vpgl_proj_camera<double> cam;
  cam_stream >> cam;
#ifdef DEBUG
  std::cout << cam << std::endl;
#endif
  return cam.clone();
}

bwm_observer_proj_cam::bwm_observer_proj_cam(bgui_image_tableau_sptr img,
                                             std::string name,
                                             std::string& image_path,
                                             std::string& cam_path,
                                             std::string& subtype,
                                             bool display_image_path)
  : bwm_observer_cam(img)
{
  subtype_ = subtype;
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
  scene_ = depth_map_scene(img_res->ni(), img_res->nj());
  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a separate file
  if (subtype!="identity"&&cam_path.size() == 0)
  {
    bwm_utils::show_error("Camera tableaus need a valid camera path!");
    return;
  }
  this->set_camera_path(cam_path);
  if (subtype=="identity")
    camera_ = new vpgl_perspective_camera<double>();
  else
    camera_ = bwm_observer_proj_cam::read_camera(cam_path,subtype,
                                                 img_res->ni(),
                                                 img_res->nj());
  if (camera_&& subtype == "perspective") {
    vpgl_perspective_camera<double>* cam =
      static_cast<vpgl_perspective_camera<double>*>(camera_);
    scene_.set_camera(*cam);
    std::string ipath = img_tab_->file_name();
    std::string img_file = vul_file::strip_directory(ipath);
    scene_.set_image_path(img_file);
  }

  //generate a unique tab name if null
  if (name=="")
    name = cam_path;
  set_tab_name(name);
  // add the observer to the observer pool
  bwm_observer_mgr::instance()->add(this);
}

bool bwm_observer_proj_cam::intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                                    vgl_plane_3d<double> plane,
                                                    vgl_point_3d<double> &world_point)
{
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  vgl_homg_point_2d<double> img_point_h(img_point);
  vgl_homg_line_3d_2_points<double> ray = proj_cam->backproject(img_point_h);
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

std::ostream& bwm_observer_proj_cam::print_camera(std::ostream& s)
{
  vpgl_proj_camera<double>* proj_cam = static_cast<vpgl_proj_camera<double> *> (camera_);
  s << *proj_cam;
  return s;
}
