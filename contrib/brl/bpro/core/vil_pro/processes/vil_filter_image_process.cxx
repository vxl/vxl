// This is brl/bpro/core/vil_pro/processes/vil_filter_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_threshold.h>

//: Constructor
bool vil_filter_image_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  //: original image
  input_types.emplace_back("vil_image_view_base_sptr");  //: filtering mask (will make all pixels with mask=false zero
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
bool vil_filter_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_filter_image_process: The input number should be 3" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_inp = pro.get_input<vil_image_view_base_sptr>(i++);

  if (mask_inp->pixel_format() != VIL_PIXEL_FORMAT_BOOL) {
    std::cout << "ERROR in vil_filter_image_process() -- input mask format is not BOOL!\n";
    return false;
  }
  vil_image_view<bool> mask(mask_inp);
  vil_image_view<vxl_byte> out_img(image);

  for (unsigned i = 0; i < image->ni(); i++)
    for (unsigned j = 0; j < image->nj(); j++)
      if (!mask(i,j))
        out_img(i, j) = 0;

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vxl_byte>(out_img));
  return true;
}
