// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_local_rational_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vpgl/bgeo/bgeo_lvcs_sptr.h>

//: initialization
bool vpgl_load_local_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs: camera and lvcs filename
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
bool vpgl_load_local_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= 1) {
    vcl_cout << "vpgl_load_local_rational_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string camera_filename = pro.get_input<vcl_string>(0);

  vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(camera_filename);
  if ( !ratcam ) {
    vcl_cerr << "Failed to load rational camera from file" << camera_filename << vcl_endl;
    return false;
  }
#if 0   // local rational camera should have lvcs info in it, no need for separate file path for lvcs
  bgeo_lvcs_sptr lvcs = new bgeo_lvcs();
  vcl_ifstream is(lvcs_filename.c_str());
  if (!is){
    vcl_cerr << " Error opening file  " << vcl_endl;
    return false;
  }
  lvcs->read(is);

  vpgl_local_rational_camera<double> locratcam(*lvcs,*ratcam);
  vbl_smart_ptr<vpgl_camera<double> > locratcamptr = new vpgl_local_rational_camera<double>(locratcam);
  pro.set_output_val<vpgl_camera_double_sptr>(0, locratcamptr);
#endif
  pro.set_output_val<vpgl_camera_double_sptr>(0, ratcam);
  return true;
}
