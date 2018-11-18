// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_to_local_rational_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/vpgl_lvcs_sptr.h>

//: initialization
bool vpgl_convert_to_local_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs and has one output
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("vpgl_lvcs_sptr");
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_to_local_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 2) {
    std::cout << "vpgl_convert_to_local_rational_camera_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(1);

  auto *rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(camera.ptr());
  if ( !rat_cam ) {
    std::cerr << "Error: camera is not a vpgl_rational_camera\n";
    return false;
  }

  if (dynamic_cast<vpgl_local_rational_camera<double>*>(rat_cam)) {
    std::cerr << "Error: rational camera is already local!\n";
    return false;
  }

  vpgl_camera_double_sptr local_ratcam = new vpgl_local_rational_camera<double>(*lvcs, *rat_cam);

  pro.set_output_val<vpgl_camera_double_sptr>(0, local_ratcam);

  return true;
}
