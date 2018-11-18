// This is brl/bpro/core/vil_pro/processes/vil_image_pair_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include "../vil_math_functors.h"
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

//: Constructor
bool vil_image_pair_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // input image
  input_types.emplace_back("vil_image_view_base_sptr"); // input image
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}


//: Execute the process
bool vil_image_pair_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_image_pair_process: The input number should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr image0 = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr image1 = pro.get_input<vil_image_view_base_sptr>(i++);

  unsigned ni0 = image0->ni(), nj0 = image0->nj();
  unsigned ni1 = image1->ni(), nj1 = image1->nj();
  unsigned ni = ni0, nj = nj0;
  if (ni>ni1) ni = ni1;
  if (nj>nj1) nj = nj1;
  unsigned separating_bar = 5;
  unsigned nip = 2*ni + separating_bar;
  //only implement for byte grey scale and byte color

  vil_pixel_format fmt0 = image0->pixel_format();
  vil_pixel_format fmt1 = image1->pixel_format();
  if (fmt0!=fmt1)
    return false;
  vil_image_view_base_sptr out_image;
  switch (fmt0)
  {
    case VIL_PIXEL_FORMAT_BYTE:
    {
      vil_image_view<vxl_byte> bimage0 = vil_convert_cast(vxl_byte(), image0);

      vil_image_view<vxl_byte> bimage1 = vil_convert_cast(vxl_byte(), image1);

      auto* comb = new vil_image_view<vxl_byte>(nip, nj);

      for (unsigned j=0; j<nj; ++j) {
        for (unsigned i=0; i<ni; ++i) {
          (*comb)(i,j) = bimage0(i, j);
          (*comb)(ni+i+separating_bar,j) = bimage1(i, j);
        }
        for (unsigned i=ni; i<ni+separating_bar; ++i)
          (*comb)(i,j) = static_cast<vxl_byte>(255);
      }
      out_image = comb;
      break;
    }
    case VIL_PIXEL_FORMAT_RGB_BYTE:
    {
      vil_image_view<vil_rgb<vxl_byte> > cimage0 =
        vil_convert_cast(vil_rgb<vxl_byte>(), image0);

      vil_image_view<vil_rgb<vxl_byte> > cimage1 =
        vil_convert_cast(vil_rgb<vxl_byte>(), image1);

      auto* comb =
        new vil_image_view<vil_rgb<vxl_byte> >(nip, nj);

      for (unsigned j=0; j<nj; ++j) {
        for (unsigned i=0; i<ni; ++i) {
          (*comb)(i,j) = cimage0(i, j);
          (*comb)(ni+i+separating_bar,j) = cimage1(i, j);
        }
        for (unsigned i=ni; i<ni+separating_bar; ++i)
          (*comb)(i,j) = vil_rgb<vxl_byte>(0, 255,0);
      }
      out_image = comb;
        break;
    }
    default:
      return false;
  }
  pro.set_output_val<vil_image_view_base_sptr>(0, out_image);
  return true;
}
