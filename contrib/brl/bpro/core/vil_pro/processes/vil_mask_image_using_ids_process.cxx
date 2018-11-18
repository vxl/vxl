// This is brl/bpro/core/vil_pro/processes/vil_mask_image_using_ids_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file  Mask all ids except the given one in an image using another id image (should be exactly same size)

#include "../vil_math_functors.h"
#include <vil/vil_convert.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view_base.h>
#include <vil/vil_transform.h>
#include <vil/vil_math.h>

//: Constructor
bool vil_mask_image_using_ids_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // input
  input_types.emplace_back("vil_image_view_base_sptr");  // mask image
  input_types.emplace_back("unsigned");   // input id (all the other ids will be masked)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}


//: Execute the process
bool vil_mask_image_using_ids_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 3) {
    std::cout << "vil_map_image_binary_process: The input number should be 3" << std::endl;
    return false;
  }

  unsigned i=0;
  vil_image_view_base_sptr image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  auto idi = pro.get_input<unsigned>(i++);
  auto id = (unsigned char)idi;

  unsigned ni = image_sptr->ni();
  unsigned nj = image_sptr->nj();
  if (ni != mask_image_sptr->ni() || nj != mask_image_sptr->nj()) {
    std::cerr << "In vil_mask_image_using_ids_process() - incompatible input images!\n";
    return false;
  }

  vil_image_view<float> image(image_sptr);
  vil_image_view<vxl_byte> mask_image(mask_image_sptr);
  vil_image_view<float> out_image(ni, nj);
  out_image.fill(-100000);

  vil_image_view<float> dest(ni, nj);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      if (id == mask_image(i,j))
        out_image(i,j) = image(i,j);
    }

  vil_image_view_base_sptr out_image_sptr = new vil_image_view<float>(out_image);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_image_sptr);
  return true;
}
