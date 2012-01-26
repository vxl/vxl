//This is brl/bpro/core/bbas_pro/processes/brad_nitf_abs_radiometric_calibration_process.cxx
//:
// \file
//   DigitalGlobe products require an absolute radiometric calibration:
//      http://www.digitalglobe.com/downloads/QuickBird_technote_raduse_v1.pdf
//   In the output image, the pixel values are top-of-atmosphere  band-integrated  radiance values 
//     with unit W/sr per m^2
//
//

#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <bprb/bprb_func_process.h>

#include <vul/vul_awk.h>
#include <vul/vul_file.h>
#include <vil/vil_convert.h>

//: set input and output types
bool brad_nitf_abs_radiometric_calibration_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr"); // cropped satellite image, 
  input_types.push_back("vcl_string"); // This is the path to .imd file that accompanies nitf files
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr"); // bits/pixel
  return pro.set_output_types(output_types);
}

bool brad_nitf_abs_radiometric_calibration_process(bprb_func_process& pro)
{

  if (pro.n_inputs()<1)
  {
    vcl_cout << pro.name() << " The input number should be " << 1 << vcl_endl;
    return false;
  }

  //get the inputs
  vil_image_view_base_sptr img_sptr = pro.get_input<vil_image_view_base_sptr>(0);
  vcl_string imd_file_path = pro.get_input<vcl_string>(1);

  vil_image_view<float> img = *vil_convert_cast(float(), img_sptr);

  vcl_string prefix = vul_file::extension(imd_file_path);
  if (prefix != ".IMD")
  {
    vcl_cout << "source image is not IMD file that accompany an NITF file\n";
    return false;
  }
  vcl_ifstream ifs( imd_file_path.c_str() );
  //: now parse the IMD file
  vul_awk awk(ifs);
  float fact = 1.0f;
  for (; awk; ++awk)
  {
    vcl_stringstream linestr(awk.line());
    vcl_string tag; 
    linestr >> tag;
    if (tag != "absCalFactor")
      continue;
    linestr >> tag;  // read =
    linestr >> fact;
    vcl_cout << "absCalFactor: " << fact << vcl_endl;
    break;
  }
  //: now just scale the image with this factor
  vil_math_scale_values(img, fact);

  //output date time info
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(img));
  return true;
}

