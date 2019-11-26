// This is brl/bpro/core/vpgl_pro/processes/vpgl_correct_rational_camera_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <bprb/bprb_parameters.h>
#include "vpgl/vpgl_camera.h"
#include "vpgl/vpgl_rational_camera.h"
#include "vpgl/vpgl_local_rational_camera.h"
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

//: initialization
bool vpgl_correct_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes 3 inputs and has 1 output
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("double");  // offset x
  input_types.emplace_back("double");  // offset y
  input_types.emplace_back("bool");    // verbose

  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_correct_rational_camera_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong inputs!!\n";
    return false;
  }

  // get the inputs
  unsigned in_i = 0;
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  auto gt_offset_u = pro.get_input<double>(in_i++);
  auto gt_offset_v = pro.get_input<double>(in_i++);
  auto verbose = pro.get_input<bool>(in_i++);

  auto* cam_local_rat = dynamic_cast<vpgl_local_rational_camera<double>*>(cam.ptr());
  if (!cam_local_rat) {
    auto* cam_rational = dynamic_cast<vpgl_rational_camera<double>*>(cam.ptr());
    if (!cam_rational) {
      std::cerr << "In vpgl_correct_rational_camera_process() input is not of type: vpgl_rational_camera<double>\n";
      return false;
    }
    else {
      if (verbose) {
        std::cout << pro.name() << ", rational camera, (off_u,off_v)="
                  << "(" << gt_offset_u << "," << gt_offset_v << ")" << std::endl;
      }
      vpgl_rational_camera<double> cam_out_rational(*cam_rational);
      double offset_u, offset_v;
      cam_out_rational.image_offset(offset_u,offset_v);
      offset_u += gt_offset_u;
      offset_v += gt_offset_v;
      cam_out_rational.set_image_offset(offset_u,offset_v);
      vpgl_camera_double_sptr camera_out = new vpgl_rational_camera<double>(cam_out_rational);
      pro.set_output_val<vpgl_camera_double_sptr>(0, camera_out);
      return true;
    }
  }

  if (verbose) {
    std::cout << pro.name() << ", LOCAL rational camera, (off_u,off_v)="
              << "(" << gt_offset_u << "," << gt_offset_v << ")" << std::endl;
  }
  vpgl_local_rational_camera<double> cam_out_local_rational(*cam_local_rat);
  double offset_u, offset_v;
  cam_out_local_rational.image_offset(offset_u,offset_v);
  offset_u += gt_offset_u;
  offset_v += gt_offset_v;
  cam_out_local_rational.set_image_offset(offset_u,offset_v);
  vpgl_camera_double_sptr camera_out = new vpgl_local_rational_camera<double>(cam_out_local_rational);
  pro.set_output_val<vpgl_camera_double_sptr>(0, camera_out);

  return true;
}


bool vpgl_get_rational_camera_offsets_process_cons(bprb_func_process& pro)
{
  //this process takes 1 inputs and has 2 output
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  std::vector<std::string> output_types;
  output_types.emplace_back("double");
  output_types.emplace_back("double");
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_get_rational_camera_offsets_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 1) {
    std::cout << "vpgl_get_rational_camera_offsets_process: The number of inputs should be 3, not " << pro.n_inputs() << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);

  auto* cam_local_rat = dynamic_cast<vpgl_local_rational_camera<double>*>(cam.ptr());
  if (!cam_local_rat) {
    auto* cam_rational = dynamic_cast<vpgl_rational_camera<double>*>(cam.ptr());
    if (!cam_rational) {
      std::cerr << "In vpgl_get_rational_camera_offsets_process() input is not of type: vpgl_rational_camera<double>\n";
      return false;
    }
    else {
      double offset_u, offset_v;
      cam_rational->image_offset(offset_u,offset_v);
      pro.set_output_val<double>(0, offset_u);
      pro.set_output_val<double>(1, offset_v);
      return true;
    }
  }

  double offset_u, offset_v;
  cam_local_rat->image_offset(offset_u,offset_v);
  pro.set_output_val<double>(0, offset_u);
  pro.set_output_val<double>(1, offset_v);

  return true;
}
