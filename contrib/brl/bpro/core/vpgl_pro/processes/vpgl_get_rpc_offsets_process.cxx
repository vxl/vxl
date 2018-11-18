// This is brl/bpro/core/vpgl_pro/processes/vpgl_get_rpc_offsets_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <bprb/bprb_parameters.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: initialization
bool vpgl_get_rpc_offsets_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("double");  // ofset x
  output_types.emplace_back("double");  // ofset y
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_get_rpc_offsets_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 1) {
    std::cout << "vpgl_get_rpc_offsets_process: The input number should be 1, not " << pro.n_inputs() << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);

  auto* cam_local_rat = dynamic_cast<vpgl_local_rational_camera<double>*>(cam.ptr());
  if (!cam_local_rat) {
    auto* cam_rational = dynamic_cast<vpgl_rational_camera<double>*>(cam.ptr());
    if (!cam_rational) {
      std::cerr << "In vpgl_correct_rational_camera_process() input is not of type: vpgl_rational_camera<double>\n";
      return false;
    }
    else {
      std::cout << "In vpgl_correct_rational_camera_process() - correcting rational camera..\n";
      vpgl_rational_camera<double> cam_out_rational(*cam_rational);
      double offset_u, offset_v;
      cam_out_rational.image_offset(offset_u,offset_v);
      pro.set_output_val<double>(0, offset_u);
      pro.set_output_val<double>(1, offset_v);
      return true;
    }
  }

  std::cout << "In vpgl_get_rpc_offsets_process() - correcting LOCAL rational camera..\n";
  vpgl_local_rational_camera<double> cam_out_local_rational(*cam_local_rat);
  double offset_u, offset_v;
  cam_out_local_rational.image_offset(offset_u,offset_v);
  pro.set_output_val<double>(0, offset_u);
  pro.set_output_val<double>(1, offset_v);
  return true;
}
