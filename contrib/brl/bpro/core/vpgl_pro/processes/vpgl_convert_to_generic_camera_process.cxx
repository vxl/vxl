// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_to_generic_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to convert perspective camera to rational camera.

#include <vcl_iostream.h>
#include <vpgl/vpgl_generic_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>


//: Init function
bool vpgl_convert_to_generic_camera_process_cons(bprb_func_process& pro)
{
  //this process takes three inputs: 
  //  0) abstract camera
  //  1) ni (# image columns)
  //  2) nj (# image rows)
  //
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  input_types.push_back("unsigned");// ni
  input_types.push_back("unsigned");// nj
  input_types.push_back("unsigned"); // level, e.g. camera needs to be scaled or not, pass the ni-nj of the original image 
  ok = pro.set_input_types(input_types);

  // in case the 5th input is not set
  brdb_value_sptr idx = new brdb_value_t<unsigned>(0);
  pro.set_input(3, idx);

  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  // label image
  output_types.push_back("unsigned");
  output_types.push_back("unsigned");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_convert_to_generic_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 4) {
    vcl_cout << "vpgl_convert_to_generic_camera_process: The input number should be 4" << vcl_endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if(!camera){
    vcl_cout<<"Null camera input\n"<<vcl_endl;
    return false;
  }
  unsigned ni = pro.get_input<unsigned>(1), nj = pro.get_input<unsigned>(2);
  unsigned level = pro.get_input<unsigned>(3);
  vpgl_generic_camera<double> gcam;
  if(!vpgl_generic_camera_compute::compute(camera, (int)ni, (int)nj, gcam, level)){
   vcl_cout<<"camera conversion failed\n"<<vcl_endl;
    return false;
  } 
  vpgl_camera_double_sptr out = new vpgl_generic_camera<double>(gcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);
  pro.set_output_val<unsigned>(1,gcam.cols());
  pro.set_output_val<unsigned>(2,gcam.rows());
  return true;
}

