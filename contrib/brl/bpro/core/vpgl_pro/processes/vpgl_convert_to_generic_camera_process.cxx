// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_to_generic_camera_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to convert perspective camera to rational camera.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_camera_convert.h>


//: Init function
bool vpgl_convert_to_generic_camera_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs and has 3 outputs:
  //  0) abstract camera
  //  1) ni (# image columns)
  //  2) nj (# image rows)
  //  3) level (the pyramid scale)
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("unsigned"); // ni
  input_types.emplace_back("unsigned"); // nj
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
bool vpgl_convert_to_generic_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 4) {
    std::cout << "vpgl_convert_to_generic_camera_process: The number of inputs should be 4" << std::endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    std::cout<<"Null camera input\n"<<std::endl;
    return false;
  }
  auto ni = pro.get_input<unsigned>(1), nj = pro.get_input<unsigned>(2);
  auto level = pro.get_input<unsigned>(3);
  vpgl_generic_camera<double> gcam;

  if (!vpgl_generic_camera_convert::convert(camera, (int)ni, (int)nj, gcam, level)) {
   std::cout<<"camera conversion failed\n"<<std::endl;
    return false;
  }
  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);
  pro.set_output_val<unsigned>(1,gcam.cols());
  pro.set_output_val<unsigned>(2,gcam.rows());
  return true;
}

//: Init function
bool vpgl_convert_to_generic_camera_w_margin_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("unsigned");// ni
  input_types.emplace_back("unsigned");// nj
  input_types.emplace_back("unsigned"); // level, e.g. camera needs to be scaled or not, pass the ni-nj of the original image
  input_types.emplace_back("int"); // margin, e.g. to create a wider expected image
  bool ok = pro.set_input_types(input_types);

  // in case the 5th input is not set
  brdb_value_sptr idx = new brdb_value_t<unsigned>(0);
  pro.set_input(3, idx);

  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  output_types.emplace_back("unsigned");
  output_types.emplace_back("unsigned");
  output_types.emplace_back("vpgl_camera_double_sptr");
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_to_generic_camera_w_margin_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 5) {
    std::cout << "vpgl_convert_to_generic_camera_w_margin_process: The number of inputs should be 5" << std::endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    std::cout<<"Null camera input\n"<<std::endl;
    return false;
  }
  auto ni = pro.get_input<unsigned>(1), nj = pro.get_input<unsigned>(2);
  auto level = pro.get_input<unsigned>(3);
  int margin = pro.get_input<int>(4);
  vpgl_generic_camera<double> gcam;

  auto *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());
  if (!cam) {
    std::cout<<"Input camera is not perspective, not implemented for other camera types!\n";
    return false;
  }

  if (!vpgl_generic_camera_convert::convert_with_margin(*cam,(int)ni,(int)nj,gcam,margin,level)) {
    std::cout<<"camera conversion failed\n"<<std::endl;
    return false;
  }
  //: adjust the calibration matrix
  auto* ncam =
    new vpgl_perspective_camera<double>(*cam);

  vnl_matrix_fixed<double, 3, 3> T(0.0);
  T[0][0] = 1.0;   T[1][1] = 1.0; T[2][2] = 1.0;
  T[0][2] = margin;   T[1][2] = margin;

  vnl_matrix_fixed<double,3,3> M = cam->get_calibration().get_matrix();
  M = T*M;
  vpgl_calibration_matrix<double> K(M);
  ncam->set_calibration(K);

  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);
  pro.set_output_val<unsigned>(1,gcam.cols());
  pro.set_output_val<unsigned>(2,gcam.rows());
  pro.set_output_val<vpgl_camera_double_sptr>(3,ncam);
  return true;
}


//: Init function
bool vpgl_write_generic_camera_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("vcl_string"); // name of output vrml file
  input_types.emplace_back("unsigned"); // name of output vrml file
  bool ok = pro.set_input_types(input_types);

  if (!ok) return ok;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_write_generic_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    std::cout << "vpgl_write_generic_camera_process: The number of inputs should be 2" << std::endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    std::cout<<"Null camera input\n"<<std::endl;
    return false;
  }
  std::string out_name = pro.get_input<std::string>(1);
  auto level = pro.get_input<unsigned>(2);

  auto* gcam = dynamic_cast<vpgl_generic_camera<double>* >(camera.ptr());

  std::ofstream ofs(out_name.c_str());
  ofs << "#VRML V2.0 utf8\n";
  gcam->print_to_vrml(level, ofs);
  ofs.close();

  return true;
}

//: fetch the ray origin and direction for a given image pixel
bool vpgl_get_generic_camera_ray_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  input_types.emplace_back("unsigned"); // u
  input_types.emplace_back("unsigned"); // v
  bool ok = pro.set_input_types(input_types);

  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("double");  // ray origin x
  output_types.emplace_back("double");  // ray origin y
  output_types.emplace_back("double");  // ray origin z
  output_types.emplace_back("double");  // ray direction x
  output_types.emplace_back("double");  // ray direction y
  output_types.emplace_back("double");  // ray direction z
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_get_generic_camera_ray_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    std::cout << "vpgl_get_generic_camera_ray_process: The number of inputs should be 3" << std::endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    std::cout<<"Null camera input\n"<<std::endl;
    return false;
  }

  auto u = pro.get_input<unsigned>(1);
  auto v = pro.get_input<unsigned>(2);

  auto* gcam = dynamic_cast<vpgl_generic_camera<double>* >(camera.ptr());
  vgl_ray_3d<double> ray = gcam->ray(u, v);

  pro.set_output_val<double>(0, ray.origin().x());
  pro.set_output_val<double>(1, ray.origin().y());
  pro.set_output_val<double>(2, ray.origin().z());
  pro.set_output_val<double>(3, ray.direction().x());
  pro.set_output_val<double>(4, ray.direction().y());
  pro.set_output_val<double>(5, ray.direction().z());
  return true;
}
