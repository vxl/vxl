//This is brl/bseg/boxm/pro/processes/boxm_camera_viewing_scene_process.cxx
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
//:
// \file
// \brief A process for generating cameras that view a scene
//
// \author J.L. Mundy
// \date January 23, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/algo/bpgl_project.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
//:global variables
namespace boxm_camera_viewing_scene_process_globals
{
  //this process takes no inputs
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 1;
}

//:sets input and output types
bool boxm_camera_viewing_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_camera_viewing_scene_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";// the scene
  input_types_[1] = "vcl_string";//the camera type, e.g. perspective
  input_types_[2] = "double";//camera elevation degrees
  input_types_[3] = "double";//camera azimuth degrees
  input_types_[4] = "double";//radius

  input_types_[5] = "unsigned";//image ni
  input_types_[6] = "unsigned";//image nj

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//:creates a scene from parameters
bool boxm_camera_viewing_scene_process(bprb_func_process& pro)
{
  if ( !pro.verify_inputs()) {
    vcl_cout << pro.name() << "boxm_camera_viewing_scene_process: invalid inputs" << vcl_endl;
    return false;
  }
  using namespace boxm_camera_viewing_scene_process_globals;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(0);
  vcl_cout<<"h";
  if (!scene_ptr) return false;
  vcl_cout<<"hi";
  vgl_box_3d<double> bb =  scene_ptr->get_world_bbox();

  vcl_string cam_type = pro.get_input<vcl_string>(1);
  if (cam_type!="vpgl_perspective_camera")
    return false; //later other camera types
  double elevation = pro.get_input<double>(2);
  double azimuth = pro.get_input<double>(3);
  double radius = pro.get_input<double>(4);

  unsigned ni = pro.get_input<unsigned>(5);
  unsigned nj = pro.get_input<unsigned>(6);
  double dni = static_cast<double>(ni), dnj = static_cast<double>(nj);
  //
  //find a camera that will project the scene bounding box
  //entirely inside the image
  //
  // 1) determine the stare point (center of bounding box)
  vgl_point_3d<double> cn = bb.centroid();
  vgl_homg_point_3d<double> stpt(cn.x(), cn.y(), cn.z());

  // 2) determine camera center
  // the viewsphere radius is set to 10x the bounding box diameter
  //double w = bb.width(), h = bb.height(), d = bb.depth();
  double r = radius;//vcl_sqrt(w*w + h*h + d*d);
  //r *=10;
  double deg_to_rad = vnl_math::pi_over_180;
  double el = elevation*deg_to_rad, az = azimuth*deg_to_rad;
  double cx = r*vcl_sin(el)*vcl_cos(az);
  double cy = r*vcl_sin(el)*vcl_sin(az);
  double cz = r*vcl_cos(el);
  vgl_point_3d<double> cent(cx+cn.x(), cy+cn.y(), cz);

  // 3) start with a unit focal length and position the camera
  vpgl_calibration_matrix<double> K(1.0, vgl_point_2d<double>(ni/2, nj/2));
  vgl_rotation_3d<double> R;
  vpgl_perspective_camera<double>* cam =
    new vpgl_perspective_camera<double>(K, cent, R);

  //stare at the center of the scene
  vgl_vector_3d<double> up(0.0, 1.0, 0.0);
  if (vcl_fabs(el)<1.0e-3)
    cam->look_at(stpt, up);
  else
    cam->look_at(stpt);

  //4) Adjust the focal length so that the box projects into the image
  // project the bounding box
  vgl_box_2d<double> image_bb = bpgl_project::project_bounding_box(*cam, bb);
  // get 2-d box diameter and image diameter
  double bw = image_bb.width(), bh = image_bb.height();
  double bd = vcl_sqrt(bw*bw + bh*bh);
  double id = vcl_sqrt(dni*dni + dnj*dnj);
  //find the adjusted focal length
  double f = id/bd;
  K.set_focal_length(f);
  cam->set_calibration(K);

  vcl_cout<<"Camera  :" <<*cam<<vcl_endl;
  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  return true;
}
