// This is brl/bpro/core/vpgl_pro/processes/vpgl_load_rational_camera_nitf_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>

//: initialization
bool vpgl_load_rational_camera_nitf_process_cons(bprb_func_process& pro)
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
bool vpgl_load_rational_camera_nitf_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    vcl_cout << "lvpgl_load_rational_camera_process: The input number should be 1" << vcl_endl;
    return false;
  }

  // get the inputs
  vcl_string nitf_image_path = pro.get_input<vcl_string>(0);

  vil_image_resource_sptr image =
        vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    vcl_cout << "NITF image load failed in vpgl_load_rational_camera_nitf_process\n";
    return 0;
  }

  vcl_string format = image->file_format();
  vcl_string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    vcl_cout << "source image is not NITF in vpgl_load_rational_camera_nitf_process\n";
    return 0;
  }

  //cast to an nitf2_image
  vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  vpgl_nitf_rational_camera *nitf_cam=new vpgl_nitf_rational_camera(nitf_image, false);

  vpgl_camera_double_sptr ratcam ( dynamic_cast<vpgl_rational_camera<double>* >(nitf_cam));

  if ( !ratcam.as_pointer() ) {
    vcl_cerr << "Failed to load rational camera from file" << nitf_image_path << vcl_endl;
    return false;
  }

  pro.set_output_val<vpgl_camera_double_sptr>(0, ratcam);

  return true;
}

