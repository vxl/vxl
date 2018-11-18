// This is brl/bpro/core/vpgl_pro/processes/vpgl_create_local_rational_camera_nitf_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <vpgl/vpgl_lvcs.h>

//: initialization
bool vpgl_create_local_rational_camera_nitf_process_cons(bprb_func_process& pro)
{
  //this process takes 2 inputs and has 1 output
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");
  input_types.emplace_back("vcl_string");
  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}


//: Execute the process
bool vpgl_create_local_rational_camera_nitf_process(bprb_func_process& pro)
{
  if (pro.n_inputs() != 2) {
    std::cout << "vpgl_create_rational_camera_nitf_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  std::string nitf_image_path = pro.get_input<std::string>(0);
  std::string lvcs_filename = pro.get_input<std::string>(1);

  vil_image_resource_sptr image =
        vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cout << "NITF image load failed in vpgl_create_local_rational_camera_nitf_process\n";
    return false;
  }

  std::string format = image->file_format();
  std::string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    std::cout << "source image is not NITF in vpgl_create_local_rational_camera_nitf_process\n";
    return false;
  }

  //cast to an nitf2_image
  auto *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  auto *nitf_cam=new vpgl_nitf_rational_camera(nitf_image, true);

  //vpgl_camera_double_sptr ratcam ( dynamic_cast<vpgl_rational_camera<double>* >(nitf_cam));

  //if ( !ratcam.as_pointer() ) {
  //  std::cerr << "Failed to load rational camera from file" << nitf_image_path << '\n';
   // return false;
  //}

  vpgl_lvcs lvcs;
  std::ifstream ifs(lvcs_filename.c_str());
  if (!ifs.good()) {
    std::cerr << "Error opening lvcs filename " << lvcs_filename << '\n';
    return false;
  }

  lvcs.read(ifs);
  vpgl_camera_double_sptr ratcam = new vpgl_local_rational_camera<double>(lvcs,*nitf_cam);

  pro.set_output_val<vpgl_camera_double_sptr>(0, ratcam);

  return true;
}
