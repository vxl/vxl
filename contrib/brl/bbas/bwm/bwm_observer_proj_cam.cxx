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
#define DEBUG

bwm_observer_proj_cam::bwm_observer_proj_cam(bgui_image_tableau_sptr img,
                                             vcl_string name,
                                             vcl_string& image_path,
                                             vcl_string& cam_path,
                                             vcl_string& subtype,
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

  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a separate file
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
// It could be that the camera is perspective and stored as vsl binary.
// with no distinguishing characteristics So the only option is to
// assume that binary perspective cameras always have the extension
// *.vsl and the user is responsible to guarantee the proper name.
vpgl_proj_camera<double>
bwm_observer_proj_cam::read_projective_camera(vcl_string cam_path)
{
  if (subtype_=="perspective"){
  vcl_string ext = vul_file_extension(cam_path);
  if (ext == ".vsl") // binary form
  {
    vpgl_perspective_camera<double> pcam;
    vsl_b_ifstream bp_in(cam_path.c_str());
    if (!bp_in){
      vcl_cerr << "In bwm_observer_proj_cam::read_projective_camera(.) -\n"
               << " invalid binary camera file " << cam_path.data() << '\n';
      return vpgl_proj_camera<double>(pcam.get_matrix());
    }
    pcam.b_read(bp_in);
    bp_in.close();
    vpgl_proj_camera<double> cam(pcam.get_matrix());
    return cam;
  }
  //An ASCII stream for perspective camera
  vpgl_perspective_camera<double> pcam;
  vcl_ifstream cam_stream(cam_path.data());
  if (!cam_stream.is_open())
  {
    vcl_cerr << "In bwm_observer_proj_cam::read_projective_camera(.) -\n"
             << " invalid camera file " << cam_path.data() << '\n';
    return vpgl_proj_camera<double>(pcam.get_matrix());
  }

  cam_stream >> pcam;
#ifdef DEBUG
  vcl_cout << pcam << vcl_endl;
#endif
  vpgl_proj_camera<double> cam(pcam.get_matrix());
  return cam;
  }
  //must be an ASCII stream for projective camera
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
