// This is brl/bseg/brec/pro/processes/brec_glitch_overlay_process.cxx

//:
// \file
// \brief A process to generate a new probability map that extends glitch detection probability over to its effective region
//
// \author Ozge Can Ozcanli
// \date December 09, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>

#include <brec/brec_glitch.h>

//: Constructor
bool brec_glitch_overlay_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame
  input_types.emplace_back("vil_image_view_base_sptr"); //input img
  input_types.emplace_back("unsigned"); // size of the inner-square for the glitch mask (e.g. 5 means we're detecting foreground islands of 5x5 on background)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output float overlayed glitch map
  output_types.emplace_back("vil_image_view_base_sptr");  // output float overlayed glitch map as stretched to byte img
  output_types.emplace_back("vil_image_view_base_sptr");  // output the glitch map as overlayed on input img
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool brec_glitch_overlay_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3){
    std::cerr << " brec_glitch_overlay_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> map = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = map.ni(), nj = map.nj();

  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);

  if (img->pixel_format() != VIL_PIXEL_FORMAT_BYTE)
    return false;
  vil_image_view<vxl_byte> input_img(img);

  auto c_size = pro.get_input<unsigned>(i++);  // center size

  vil_image_view<float> out(ni, nj, 1);
  brec_glitch::extend_prob_to_square_region(c_size, map, out);

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(out));

  vil_image_view<vxl_byte> out2(ni, nj, 3);
  if (input_img.nplanes() == 3) {
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        out2(i,j,0) = input_img(i,j,0);
        out2(i,j,1) = input_img(i,j,1);
        out2(i,j,2) = (vxl_byte)(std::floor(out(i,j)*255.0f+0.5f));
      }
  } else {
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        out2(i,j,0) = input_img(i,j,0);
        out2(i,j,1) = input_img(i,j,0);
        out2(i,j,2) = (vxl_byte)(std::floor(out(i,j)*255.0f+0.5f));
      }
  }

  float min, max;
  vil_math_value_range(out, min, max);
  std::cout << "\t glitch map overlayed min: " << min << " max: " << max << std::endl;
  vil_image_view<vxl_byte> out_byte(ni, nj, 1);
  vil_convert_stretch_range_limited(out, out_byte, 0.0f, max);

  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<vxl_byte>(out_byte));
  pro.set_output_val<vil_image_view_base_sptr>(2, new vil_image_view<vxl_byte>(out2));

  return true;
}
