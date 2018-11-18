//This is brl/bpro/core/vil_pro/processes/vil_nitf_bits_per_pixel_process.cxx
//:
// \file

#include <iostream>
#include <cmath>
#include <vector>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_func_process.h>
//: set input and output types
bool vil_nitf_bits_per_pixel_process_cons(bprb_func_process& pro)
{
  //input, a path to an nitf image
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); // 0 nitf path
  if (!pro.set_input_types(input_types))
    return false;

  //output, extracts IDATIM property from the NITF image subheader
  std::vector<std::string> output_types;
  output_types.emplace_back("int"); // bits/pixel
  return pro.set_output_types(output_types);
}

bool vil_nitf_bits_per_pixel_process(bprb_func_process& pro)
{

  if (pro.n_inputs()<1)
  {
    std::cout << pro.name() << " The input number should be " << 1 << std::endl;
    return false;
  }

  //get the inputs
  std::string nitf_image_path = pro.get_input<std::string>(0);
  //read and parse NITF image
  vil_image_resource_sptr image =
      vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cout << "NITF image load failed in vil_nitf_date_time_process\n";
    return false;
  }

  std::string format = image->file_format();
  std::string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    std::cout << "source image is not NITF in vil_nitf_date_time_process\n";
    return false;
  }

    //cast to an nitf2_image
  auto *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());

    //get NITF information
  std::vector< vil_nitf2_image_subheader* > headers = nitf_image->get_image_headers();
  // Note, assumes the the file contains exactly one image
  vil_nitf2_image_subheader* hdr = headers[0];
  if(!hdr){
    std::cerr << "In vil_nitf_date_time_process: null image subheader\n";
    return false;
  }
  int bits = (int)hdr->get_number_of_bits_per_pixel();
  if (!bits) {
    std::cerr << "In vil_nitf_date_time_process: failed to obtain number of bits per pixel info\n";
    return false;
  }
  //output date time info
  pro.set_output_val<int>(0, bits);
  return true;
}
