// This is brl/bpro/core/vpgl_pro/processes/vpgl_get_perspective_cam_center_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vsl/vsl_binary_io.h>

//: Init function
bool vpgl_get_perspective_cam_center_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vpgl_camera_double_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("float"); // x
  output_types.push_back("float"); // y
  output_types.push_back("float"); // z
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_get_perspective_cam_center_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "vpgl_get_perspective_cam_center_process: The number of inputs should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr cam_ptr = pro.get_input<vpgl_camera_double_sptr>(0);
  vpgl_perspective_camera<double>* cam = dynamic_cast<vpgl_perspective_camera<double>*>(cam_ptr.ptr());
  if (!cam) {
    vcl_cerr << "vpgl_get_view_direction_at_point_process: couldn't cast camera\n";
    return false;
  }

  vgl_point_3d<double> cent = cam->get_camera_center();
  pro.set_output_val<float>(0, (float)cent.x());
  pro.set_output_val<float>(1, (float)cent.y());
  pro.set_output_val<float>(2, (float)cent.z());

  return true;
}

