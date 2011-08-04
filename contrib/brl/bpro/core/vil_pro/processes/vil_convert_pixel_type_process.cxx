// This is brl/bpro/core/vil_pro/processes/vil_convert_pixel_type_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Converts floating point image (0,1.0) to byte image (0,255) for compression/saveability reasons

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>

//: Constructor
bool vil_convert_pixel_type_process_cons(bprb_func_process& pro)
{
  //this process takes two inputs:
  // input(0): the vil_image_view_base_sptr
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vil_image_view_base_sptr");
  input_types.push_back("vcl_string");
  bool ok = pro.set_input_types(input_types);

  //this process has 1 outputs
  // output(0): the output image with the specified number of planes
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_convert_pixel_type_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()<2) {
    vcl_cout << "vil_convert_pixel_type_process: The input number should be 2" << vcl_endl;
    return false;
  }
  unsigned i=0;

  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);

  //retrieve output type
  vcl_string out_type = pro.get_input<vcl_string>(i++);

  ////////////////////////////////////////////////////////////////////
  //Convert to float image
  ////////////////////////////////////////////////////////////////////
  vil_image_view<float> fimage;
  if (img->pixel_format() == VIL_PIXEL_FORMAT_RGBA_BYTE ) {
    vil_image_view_base_sptr plane_image = vil_convert_to_n_planes(4, img);
    fimage = *vil_convert_cast(float(), plane_image);
    vil_math_scale_values(fimage, 1.0/255.0);
  }
  else {
    //can use convert cast
    fimage = *vil_convert_cast(float(), img);

    // convert input image to float in range from 0 to 1
    if (img->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
      vil_math_scale_values(fimage,1.0/255.0);
    if (img->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
      vil_math_scale_values(fimage,1.0/65536);
  }
  unsigned ni = fimage.ni();
  unsigned nj = fimage.nj();
  unsigned nplanes = fimage.nplanes();

  ////////////////////////////////////////////////////////////////////
  //return image of same type (convert back)
  ////////////////////////////////////////////////////////////////////
  if (out_type=="float") {
    vil_image_view<float>* bimage = new vil_image_view<float>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
    return true;
  }
  else if (out_type=="byte") {
    vil_math_scale_values( fimage, 255.0 );
    vil_image_view<vxl_byte>* bimage = new vil_image_view<vxl_byte>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
    return true;
  }
  else if (out_type=="uint16") {
    vil_math_scale_values( fimage, 65535.0 );
    vil_image_view<vxl_uint_16>* bimage = new vil_image_view<vxl_uint_16>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, bimage);
    return true;
  }
  else if (out_type=="rgba")    {
    vil_math_scale_values( fimage, 255.0);
    vil_image_view<vxl_byte>* bimage = new vil_image_view<vxl_byte>(ni, nj, nplanes);
    vil_convert_cast(fimage, *bimage);
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_convert_to_component_order(bimage));
    return true;
  }
  else
    return false;
}

