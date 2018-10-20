#include "bwm_observer_geo_cam.h"
#include "bwm_observer_mgr.h"
#include "algo/bwm_utils.h"
#include <vul/vul_file.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vpgl/io/vpgl_io_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vnl/vnl_inverse.h>

vpgl_camera<double>* bwm_observer_geo_cam::
read_camera(std::string cam_path)
{
  std::string ext = vul_file_extension(cam_path);
  if (ext == ".tfw")
  {
    std::ifstream cam_stream(cam_path.c_str());
    if (!cam_stream) {
      std::cerr << "In bwm_observer_geo_cam::read_camera(.) -\n"
               << " invalid binary camera file " << cam_path.data() << '\n';
      return nullptr;
    }
    vnl_matrix<double> trans(2,2);
    cam_stream >> trans;

    vnl_matrix<double> transinv=vnl_inverse(trans);

    vnl_vector_fixed<double,4> r1(transinv(0,0),transinv(0,1),0,0);
    vnl_vector_fixed<double,4> r2(transinv(1,0),transinv(1,1),0,0);
    vpgl_affine_camera<double> * affine_cam = new vpgl_affine_camera<double>(r1,r2);

    std::cout<<*affine_cam;
    return affine_cam;
  }
  else
    return static_cast<vpgl_camera<double>*>(nullptr);
}

bwm_observer_geo_cam::bwm_observer_geo_cam(bgui_image_tableau_sptr img,
                                           std::string name,
                                           std::string& image_path,
                                           std::string& cam_path,
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
  // info is in the image, not a separate file
  if (cam_path.size() == 0)
  {
    bwm_utils::show_error("Camera tableaus need a valid camera path!");
    return;
  }
  this->set_camera_path(cam_path);
  camera_ = bwm_observer_geo_cam::read_camera(cam_path);
  //generate a unique tab name if null
  if (name=="")
    name = cam_path;
  set_tab_name(name);
  // add the observer to the observer pool
  bwm_observer_mgr::instance()->add(this);
}

bool bwm_observer_geo_cam::intersect_ray_and_plane(vgl_point_2d<double> img_point,
                                                   vgl_plane_3d<double> plane,
                                                   vgl_point_3d<double> &world_point)
{
  vpgl_affine_camera<double> * geo_cam = static_cast<vpgl_affine_camera<double> *> (camera_);
  vgl_homg_line_3d_2_points<double> ray = geo_cam->backproject(vgl_homg_point_2d<double>(img_point));
  vgl_homg_operators_3d<double> oper;
  vgl_homg_point_3d<double> p = oper.intersect_line_and_plane(ray,plane);
  std::cout<<"World Point "<<world_point<<'\n';
  world_point = p;
  return true;
}

std::ostream& bwm_observer_geo_cam::print_camera(std::ostream& s)
{
  vpgl_affine_camera<double> * geo_cam = static_cast<vpgl_affine_camera<double> * > (camera_);
  s << *geo_cam;
  return s;
}
