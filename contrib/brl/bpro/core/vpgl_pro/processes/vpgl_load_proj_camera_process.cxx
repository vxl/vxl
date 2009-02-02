// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_proj_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vnl/vnl_matrix_fixed.h>

//: Constructor
bool vpgl_load_proj_camera_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); 
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  
  return true;
}


//: Execute the process
bool vpgl_load_proj_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "lvpgl_load_proj_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string camera_filename = pro.get_input<vcl_string>(0);

  // read projection matrix from the file.
  vcl_ifstream ifs(camera_filename.c_str());
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camera_filename << vcl_endl;
    return false;
  }
  vnl_matrix_fixed<double,3,4> projection_matrix;
  ifs >> projection_matrix;

  vbl_smart_ptr<vpgl_camera<double> > procam = new vpgl_proj_camera<double>(projection_matrix);

  pro.set_output_val<vpgl_camera_double_sptr>(0, procam);

  return true;
}

