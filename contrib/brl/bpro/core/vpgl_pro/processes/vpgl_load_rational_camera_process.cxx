// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_rational_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: initialization
bool vpgl_load_rational_camera_process_cons(bprb_func_process& pro)
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
bool vpgl_load_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "lvpgl_load_rational_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }
  
  // get the inputs
  vcl_string camera_filename = pro.get_input<vcl_string>(0);

  vpgl_camera_double_sptr ratcam = read_local_rational_camera<double>(camera_filename);

  if ( !ratcam.as_pointer() ) {
    //vcl_cerr << "Rational camera isn't local... trying global" << vcl_endl;
    ratcam = read_rational_camera<double>(camera_filename);

  }
  
  if ( !ratcam.as_pointer() ) {
    vcl_cerr << "Failed to load rational camera from file" << camera_filename << vcl_endl;
    return false;
  }

  pro.set_output_val<vpgl_camera_double_sptr>(0, ratcam);

  return true;
}

