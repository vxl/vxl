// This is brl/bseg/boxm/algo/rt/pro/processes/boxm_estimate_camera_process.cxx
//:
// \file
#include <iostream>
#include <cstdio>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <brip/brip_vil_float_ops.h>

#include <vnl/algo/vnl_amoeba.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/bpgl_camera_estimator.h>
#include <bpgl/bpgl_camera_estimator_amoeba.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <boxm/algo/rt/boxm_expected_edge_functor.h>

//: globals
namespace boxm_estimate_camera_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 3;

  // parameter strings
  const std::string theta_range_ =  "theta_range";
  const std::string theta_step_ =  "theta_step";
  const std::string phi_range_ =  "phi_range";
  const std::string phi_step_ =  "phi_step";
  const std::string rot_range_ =  "rot_range";
  const std::string rot_step_ =  "rot_step";
  const std::string max_iter_rot_angle_ =  "max_iter_rot_angle";
  const std::string max_iter_cam_center_ =  "max_iter_cam_center";
}

//: set input and output types
bool boxm_estimate_camera_process_cons(bprb_func_process& pro)
{
  using namespace boxm_estimate_camera_process_globals;

  // process takes 3 inputs:
  //input[0]: The scene
  //input[1]: Initial camera
  //input[2]: Edge image

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "boxm_scene_base_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 3 outputs:
  // output[0]: The optimized camera
  // output[1]: Expected edge image (after camera correction)
  // output[2]: Expected edge image (before camera correction)

  std::vector<std::string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "vpgl_camera_double_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  output_types_[j++] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//:  optimizes camera parameters based on edges
bool boxm_estimate_camera_process(bprb_func_process& pro)
{
  using namespace boxm_estimate_camera_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;

  // scene
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);

  // camera
  vpgl_camera_double_sptr cam_inp = pro.get_input<vpgl_camera_double_sptr>(i++);
  auto *cam_init = dynamic_cast<vpgl_perspective_camera<double>*>(cam_inp.ptr());

  // image
  vil_image_view_base_sptr img_e_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> img_e_vb(img_e_sptr);

  vil_image_view<float> img_e;
  brip_vil_float_ops::normalize_to_interval<vxl_byte,float>(img_e_vb,img_e,0.0f,1.0f);

  unsigned ni = img_e.ni();
  unsigned nj = img_e.nj();

  double deg2rad = vnl_math::pi_over_180;

  // parameters are in degrees
  double theta_range = 12.0, theta_step = 0.05, phi_range = 12.0, phi_step = 0.05, rot_range = 10.0, rot_step = 1.0;
  int max_iter_rot_angle = 5, max_iter_cam_center = 50;

  pro.parameters()->get_value(theta_range_, theta_range);
  pro.parameters()->get_value(theta_step_, theta_step);
  pro.parameters()->get_value(phi_range_, phi_range);
  pro.parameters()->get_value(phi_step_, phi_step);
  pro.parameters()->get_value(rot_range_, rot_range);
  pro.parameters()->get_value(rot_step_, rot_step);
  pro.parameters()->get_value(max_iter_rot_angle_, max_iter_rot_angle);
  pro.parameters()->get_value(max_iter_cam_center_, max_iter_cam_center);

#if 0
  std::cout << "printing boxm_estimate_camera_process parameters:\n"
           << "theta_range: " << theta_range << '\n'
           << "theta_step: " << theta_step << '\n'
           << "phi_range: " << phi_range << '\n'
           << "phi_step: " << phi_step << '\n'
           << "rot_range: " << rot_range << '\n'
           << "rot_step: " << rot_step << '\n'
           << "max_iter_rot_angle: " << max_iter_rot_angle << '\n'
           << "max_iter_cam_center: " << max_iter_cam_center << '\n';
#endif

  // change the parameters into radians
  theta_range *= deg2rad; theta_step *= deg2rad; phi_range *= deg2rad; phi_step *= deg2rad; rot_range *= deg2rad; rot_step *= deg2rad;

  auto *img_eei = new vil_image_view<float>(ni,nj,1);
  img_eei->fill(0.0f);

  if (scene_ptr->appearence_model() == BOXM_EDGE_LINE) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_inf_line_sample<float> > type;
      auto* scene = dynamic_cast<boxm_scene<type>*> (scene_ptr.as_pointer());
      if (!scene) {
        std::cerr << "boxm_render_expected_edge_process: the scene is not of expected type\n";
        return false;
      }

      // create the functor that finds edges
      typedef boxm_expected_edge_functor<short, boxm_inf_line_sample<float> > func_type;
      func_type func(*scene);
      bpgl_camera_estimator<func_type > cam_estimator(func);
      cam_estimator.set_estimation_params(theta_range,theta_step,phi_range,phi_step,rot_range,rot_step,max_iter_rot_angle);

      func.apply(cam_inp,img_eei);

      auto *img_eei_before_correction = new vil_image_view<vxl_byte>(ni,nj,1);
      brip_vil_float_ops::normalize_to_interval<float,vxl_byte>(*img_eei,*img_eei_before_correction,0.0f,255.0f);

      bpgl_camera_estimator_amoeba<func_type> cost_ftn(cam_estimator,img_e,cam_inp);
      vnl_vector<double> x(2,1.0);
      vnl_amoeba amoeba(cost_ftn);
      amoeba.verbose = 1;
      amoeba.default_verbose = true;
      amoeba.set_relative_diameter(3.0);
      amoeba.set_max_iterations(max_iter_cam_center);
      amoeba.minimize(x);
      auto *cam_est = new vpgl_perspective_camera<double>(*cam_init);
      cost_ftn.get_result(x,cam_est);

      vpgl_camera_double_sptr cam_ptr = new vpgl_perspective_camera<double>(*cam_est);
      func.apply(cam_ptr,img_eei);

      auto *img_eei_vb = new vil_image_view<vxl_byte>(ni,nj,1);
      brip_vil_float_ops::normalize_to_interval<float,vxl_byte>(*img_eei,*img_eei_vb,0.0f,255.0f);

      delete cam_est;
      delete img_eei;

      // output
      unsigned j = 0;
      // update the camera and store
      pro.set_output_val<vpgl_camera_double_sptr>(j++, cam_ptr);
      // update the edge image after update and store
      pro.set_output_val<vil_image_view_base_sptr>(j++, img_eei_vb);
      // update the edge image before update and store
      pro.set_output_val<vil_image_view_base_sptr>(j++, img_eei_before_correction);

      return true;
    }
    else
    {
      std::cerr << "boxm_estimate_camera_process: Multibin version not yet implemented\n";
      return false;
    }
  }
  else {
    std::cerr << "boxm_estimate_camera_process: undefined APM type\n";
    return false;
  }
}
