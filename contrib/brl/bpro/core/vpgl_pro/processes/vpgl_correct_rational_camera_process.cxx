// This is brl/bpro/core/vpgl_pro/processes/vpgl_correct_rational_camera_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: initialization
bool vpgl_correct_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr"); 
  input_types.push_back("double");  // ofset x
  input_types.push_back("double");  // ofset y
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vpgl_camera_double_sptr"); 
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  
  return true;

}

//: Execute the process
bool vpgl_correct_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "lvpgl_load_rational_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(0);
  double gt_offset_u = pro.get_input<double>(1);
  double gt_offset_v = pro.get_input<double>(2);

  vpgl_local_rational_camera<double>* cam_local_rat = dynamic_cast<vpgl_local_rational_camera<double>*>(cam.ptr());
  if (!cam_local_rat) {
    vpgl_rational_camera<double>* cam_rational = dynamic_cast<vpgl_rational_camera<double>*>(cam.ptr());
    if (!cam_rational) {
      vcl_cerr << "In vpgl_correct_rational_camera_process() input is not of type: vpgl_rational_camera<double>\n";
      return false;
    } else {
      vcl_cout << "In vpgl_correct_rational_camera_process() - correcting rational camera..\n";
      vpgl_rational_camera<double> cam_out_rational(*cam_rational);
      double offset_u, offset_v;
      cam_out_rational.image_offset(offset_u,offset_v);
      offset_u += gt_offset_u;
      offset_v += gt_offset_v;
      cam_out_rational.set_image_offset(offset_u,offset_v);
      vpgl_camera_double_sptr camera_out = new vpgl_rational_camera<double>(cam_out_rational);
      pro.set_output_val<vpgl_camera_double_sptr>(0, camera_out);
      return true;
    }
  } 

  vcl_cout << "In vpgl_correct_rational_camera_process() - correcting LOCAL rational camera..\n";
  vpgl_local_rational_camera<double> cam_out_local_rational(*cam_local_rat);
  double offset_u, offset_v;
  cam_out_local_rational.image_offset(offset_u,offset_v);
  offset_u += gt_offset_u;
  offset_v += gt_offset_v;
  cam_out_local_rational.set_image_offset(offset_u,offset_v);
  vpgl_camera_double_sptr camera_out = new vpgl_local_rational_camera<double>(cam_out_local_rational);
  pro.set_output_val<vpgl_camera_double_sptr>(0, camera_out);
  
  return true;
}

