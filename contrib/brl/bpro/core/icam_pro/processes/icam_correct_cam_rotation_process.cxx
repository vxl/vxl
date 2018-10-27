// This is brl/bpro/core/icam_pro/processes/icam_correct_cam_rotation_process.cxx

//:
// \file
// \brief A process for correcting a camera by registering two images,
//           e.g. an input image and an associated camera is given. the camera principal axis needs to be rotated
//                by some amount so that the input image matches to the expected image (second input image of the process)
//                given by a 3d model of the scene, rendered by the input camera
//
// \author Ozge C. Ozcanli
// \date February 16, 2012
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <icam/icam_minimizer.h>
#include <vpgl/algo/vpgl_ray.h>


// global variables
namespace icam_correct_cam_rotation_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 2;
}

//: sets input and output types
bool icam_correct_cam_rotation_process_cons(bprb_func_process& pro)
{
  using namespace icam_correct_cam_rotation_process_globals;
  unsigned i=0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "vil_image_view_base_sptr";  // input image, dest_image
  input_types_[i++] = "vpgl_camera_double_sptr";   // input camera, needs to be corrected
  input_types_[i++] = "vil_image_view_base_sptr";  // expected image given by input camera, needs to be larger than input image
                                                   // source_image
  input_types_[i++] = "float";                     // cone half angle, in degrees
  input_types_[i++] = "unsigned";                  // n steps along the spiral that samples the cone
  input_types_[i++] = "bool";                      // if true refine using Powell, default is to refine

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr"; // mapped image
  output_types_[1] = "vpgl_camera_double_sptr"; // corrected output camera

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: creates a scene from parameters
bool icam_correct_cam_rotation_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << "icam_correct_cam_rotation_process: invalid inputs" << std::endl;
    return false;
  }
  using namespace icam_correct_cam_rotation_process_globals;

  int i=0;
  vil_image_view_base_sptr dest_img_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr source_img_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  double cone_half_angle = double(pro.get_input<float>(i++))*vnl_math::pi_over_180; //double cone_half_angle = 0.15;
  auto n_axis_steps = pro.get_input<unsigned>(i++);
  bool refine = pro.get_input<bool>(i++);

  vil_image_view<float> dest_img(dest_img_sptr->ni(), dest_img_sptr->nj());
  if (dest_img_sptr->nplanes() == 3) {
    auto* inimg = dynamic_cast<vil_image_view<vxl_byte>* >(dest_img_sptr.ptr());
    vil_convert_planes_to_grey<vxl_byte, float>(*inimg, dest_img);
  } else {
    dest_img = vil_convert_cast(float(), dest_img_sptr);
  }
  vil_image_view<float> source_img(source_img_sptr->ni(), source_img_sptr->nj());
  if (source_img_sptr->nplanes() == 3) {
    auto* inimg = dynamic_cast<vil_image_view<vxl_byte>* >(source_img_sptr.ptr());
    vil_convert_planes_to_grey<vxl_byte, float>(*inimg, source_img);
  } else {
    source_img = vil_convert_cast(float(), source_img_sptr);
  }

  if (!dest_img||!source_img)
    return false;

  //also to get the calibration matrix, K
  auto* pers_cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam.ptr());
  if (!pers_cam)
    return false;
  vnl_double_3x3 K = pers_cam->get_calibration().get_matrix();
  //make a uniform depth image since the search is over only rotation
  vil_image_view<double> depth(dest_img.ni(), dest_img.nj());
  depth.fill(1.0);
  vgl_rotation_3d<double> rot, min_rot;
  //solve only for rotation translation = 0
  vgl_vector_3d<double> tr(0, 0, 0);
  //form the depth transform (rotation only)
  icam_depth_transform dt(K, depth, rot, tr);

  // set up the minimizer (default params)
  icam_minimizer_params icam_params;
  icam_minimizer minimizer(source_img, dest_img, dt, icam_params, true);

  // search over a set of principal axes within the specified cone
  double min_cost, min_overlap;
  minimizer.rot_search(tr, rot, n_axis_steps, cone_half_angle, 0,
                       0.0, 0, 0.5, min_rot, min_cost, min_overlap);

  vgl_rotation_3d<double> Rp = pers_cam->get_rotation();
  vgl_rotation_3d<double> Rrp = min_rot.inverse()*Rp;
  auto* out_cam1 = new vpgl_perspective_camera<double>();
  auto* out_cam = new vpgl_perspective_camera<double>();
  out_cam1->set_calibration(pers_cam->get_calibration());
  out_cam1->set_rotation(Rrp);
  out_cam1->set_camera_center(pers_cam->get_camera_center());
  if (!refine) {
    std::cout << "NOT REFINING!\n";
    vil_image_view<float> mapped_source = minimizer.view(min_rot, tr, 0);
    pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(mapped_source));
    pro.set_output_val<vpgl_camera_double_sptr>(1, out_cam1);
    return true;
  }

  // refine the rotation using the Powell algorithm
  double min_allowed_overlap = 0.25;
  unsigned pyramid_level = 0;
  minimizer.minimize_rot(min_rot, tr, pyramid_level, min_allowed_overlap);
  vil_image_view<float> mapped_source = minimizer.view(min_rot, tr, 0);
  std::cout << "min_cost: " << min_cost << " after powell error: " << minimizer.end_error() << "\n";

  Rrp = min_rot.inverse()*Rp;

  out_cam->set_calibration(pers_cam->get_calibration());
  out_cam->set_rotation(Rrp);
  out_cam->set_camera_center(pers_cam->get_camera_center());

  double dist = vpgl_persp_cam_distance(*pers_cam, *out_cam);
  if (dist > 2*cone_half_angle) {
    out_cam = out_cam1;
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(mapped_source));
  pro.set_output_val<vpgl_camera_double_sptr>(1, out_cam);
  return true;
}
