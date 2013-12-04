// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_to_generic_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to convert perspective camera to rational camera.

#include <vcl_iostream.h>
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
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("unsigned"); // ni
  input_types.push_back("unsigned"); // nj
  input_types.push_back("unsigned"); // level, e.g. camera needs to be scaled or not, pass the ni-nj of the original image
  bool ok = pro.set_input_types(input_types);

  // in case the 4th input is not set
  brdb_value_sptr idx = new brdb_value_t<unsigned>(0);
  pro.set_input(3, idx);

  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  // label image
  output_types.push_back("unsigned");
  output_types.push_back("unsigned");
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_to_generic_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 4) {
    vcl_cout << "vpgl_convert_to_generic_camera_process: The number of inputs should be 4" << vcl_endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    vcl_cout<<"Null camera input\n"<<vcl_endl;
    return false;
  }
  unsigned ni = pro.get_input<unsigned>(1), nj = pro.get_input<unsigned>(2);
  unsigned level = pro.get_input<unsigned>(3);
  vpgl_generic_camera<double> gcam;

  if (!vpgl_generic_camera_convert::convert(camera, (int)ni, (int)nj, gcam, level)) {
   vcl_cout<<"camera conversion failed\n"<<vcl_endl;
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
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("unsigned");// ni
  input_types.push_back("unsigned");// nj
  input_types.push_back("unsigned"); // level, e.g. camera needs to be scaled or not, pass the ni-nj of the original image
  input_types.push_back("int"); // margin, e.g. to create a wider expected image
  bool ok = pro.set_input_types(input_types);

  // in case the 5th input is not set
  brdb_value_sptr idx = new brdb_value_t<unsigned>(0);
  pro.set_input(3, idx);

  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  // label image
  output_types.push_back("unsigned");
  output_types.push_back("unsigned");
  output_types.push_back("vpgl_camera_double_sptr");
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_to_generic_camera_w_margin_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 5) {
    vcl_cout << "vpgl_convert_to_generic_camera_w_margin_process: The number of inputs should be 5" << vcl_endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    vcl_cout<<"Null camera input\n"<<vcl_endl;
    return false;
  }
  unsigned ni = pro.get_input<unsigned>(1), nj = pro.get_input<unsigned>(2);
  unsigned level = pro.get_input<unsigned>(3);
  int margin = pro.get_input<int>(4);
  vpgl_generic_camera<double> gcam;

  vpgl_perspective_camera<double> *cam = dynamic_cast<vpgl_perspective_camera<double>*>(camera.as_pointer());
  if (!cam) {
    vcl_cout<<"Input camera is not perspective, not implemented for other camera types!\n";
    return false;
  }

  if (!vpgl_generic_camera_convert::convert_with_margin(*cam,(int)ni,(int)nj,gcam,margin,level)) {
    vcl_cout<<"camera conversion failed\n"<<vcl_endl;
    return false;
  }
  //: adjust the calibration matrix
  vpgl_perspective_camera<double>* ncam =
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
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("vcl_string"); // name of output vrml file
  input_types.push_back("unsigned"); // name of output vrml file
  bool ok = pro.set_input_types(input_types);

  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_write_generic_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 3) {
    vcl_cout << "vpgl_write_generic_camera_process: The number of inputs should be 2" << vcl_endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    vcl_cout<<"Null camera input\n"<<vcl_endl;
    return false;
  }
  vcl_string out_name = pro.get_input<vcl_string>(1);
  unsigned level = pro.get_input<unsigned>(2);

  vpgl_generic_camera<double>* gcam = dynamic_cast<vpgl_generic_camera<double>* >(camera.ptr());

  vcl_ofstream ofs(out_name.c_str());
  ofs << "#VRML V2.0 utf8\n";
  gcam->print_to_vrml(level, ofs);
  ofs.close();

  return true;
}
