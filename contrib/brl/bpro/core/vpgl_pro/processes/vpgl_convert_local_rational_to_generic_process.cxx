// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_local_rational_to_generic_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to convert a local_rational_camera to a generic_camera, using user-defined min and max z planes.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_camera_convert.h>


//: Init function
bool vpgl_convert_local_rational_to_generic_process_cons(bprb_func_process& pro)
{
  //this process takes 6 inputs and has 3 outputs:
  //  0) abstract camera
  //  1) ni (# image columns)
  //  2) nj (# image rows)
  //  3) local z minimum
  //  4) local z maximum
  //  5) level (the pyramid scale)
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("unsigned");// ni
  input_types.emplace_back("unsigned");// nj
  input_types.emplace_back("float"); // min z
  input_types.emplace_back("float"); // max z
  input_types.emplace_back("unsigned"); // level, e.g. camera needs to be scaled or not, pass the ni-nj of the original image
  bool ok = pro.set_input_types(input_types);

  // in case the 4th input is not set
  brdb_value_sptr idx = new brdb_value_t<unsigned>(0);
  pro.set_input(3, idx);

  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  output_types.emplace_back("unsigned");
  output_types.emplace_back("unsigned");
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_local_rational_to_generic_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    std::cout << "vpgl_convert_local_rational_to_generic_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    std::cerr << "Null camera input\n\n";
    return false;
  }
  auto *lrat_cam = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr());
  if (!lrat_cam) {
    std::cerr << "Error: camera is not a vpgl_local_rational_camera\n";
    return false;
  }

  auto ni = pro.get_input<unsigned>(1);
  auto nj = pro.get_input<unsigned>(2);

  double min_z = pro.get_input<float>(3);
  double max_z = pro.get_input<float>(4);

  auto level = pro.get_input<unsigned>(5);
  vpgl_generic_camera<double> gcam;

  if (!vpgl_generic_camera_convert::convert(*lrat_cam, (int)ni, (int)nj, gcam, min_z, max_z, level)) {
   std::cout<<"camera conversion failed\n"<<std::endl;
    return false;
  }
  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);
  pro.set_output_val<unsigned>(1,gcam.cols());
  pro.set_output_val<unsigned>(2,gcam.rows());
  return true;
}
