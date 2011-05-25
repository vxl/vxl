//This is brl/bpro/core/bbas_pro/processes/bbas_sun_angles_process.cxx
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>

#include <vcl_vector.h>
#include <vcl_iostream.h>

//: set input and output types
bool bbas_sun_angles_process_cons(bprb_func_process& pro)
{
  //inputs
  // 0: NITF image path
  vcl_vector<vcl_string> input_types_(1);
  input_types_[0] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  //output
   // 0: sun elevation
   // 1: sun azimuth
  vcl_vector<vcl_string> output_types_(2);
  output_types_[0] = "float";  
  output_types_[1] = "float";
  return pro.set_output_types(output_types_);
}

bool bbas_sun_angles_process(bprb_func_process& pro)
{
  if (pro.n_inputs()<1)
  {
    vcl_cout << pro.name() << " The input number should be 1 " << vcl_endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vcl_string nitf_image_path = pro.get_input<vcl_string>(i++);

  //read and parse NITF image

  vil_image_resource_sptr image =
      vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    vcl_cout << "NITF image load failed in bvxm_illum_index_process\n";
    return 0;
  }

  vcl_string format = image->file_format();
  vcl_string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    vcl_cout << "source image is not NITF in bbas_sun_angles_process\n";
    return 0;
  }

  //cast to an nitf2_image
  vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());

  //get NITF information
  vcl_vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  vil_nitf2_image_subheader* hdr = headers[0];

  double sun_el;
  double sun_az;

  bool success = hdr->get_sun_params(sun_el, sun_az);

  if (!success) {
    vcl_cerr << "error bbas_sun_angles_process: failed to obtain illumination angles from nitf image\n";
    return false;
  }

  //store bin index
  pro.set_output_val<float>(0, (float)sun_el);
  pro.set_output_val<float>(1, (float)sun_az);

  return true;
}
