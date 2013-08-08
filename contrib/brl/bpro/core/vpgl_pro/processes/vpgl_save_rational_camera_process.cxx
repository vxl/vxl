// This is brl/bpro/core/vpgl_pro/processes/vpgl_save_rational_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: Init function
bool vpgl_save_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  //input[0]: the camera
  //input[1]: the filename
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr"); 
  input_types.push_back("vcl_string"); 
  return pro.set_input_types(input_types);
}


//: Execute the process
bool vpgl_save_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    vcl_cout << "vpgl_save_rational_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  vcl_string camera_filename = pro.get_input<vcl_string>(1);

  vpgl_local_rational_camera<double> *cam = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.as_pointer());
  
  if (!cam) {
   vpgl_rational_camera<double> *cam2 = dynamic_cast<vpgl_rational_camera<double>*>(camera.as_pointer());

    if (!cam2) {
      vcl_cerr << "error: could not convert camera input to a vpgl_rational_camera or local rational camera\n";
      return false;
    }
    
    if (!cam2->save(camera_filename)) {
      vcl_cerr << "Failed to save file " << camera_filename << vcl_endl;
      return false;
    }
  } else {
    if (!cam->save(camera_filename)) {
      vcl_cerr << "Failed to save file " << camera_filename << vcl_endl;
    }
  }

  return true;
}

