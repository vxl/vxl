// This is brl/bpro/core/vil_pro/processes/vil_rgbi_to_grey_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: Constructor
bool vil_rgbi_to_grey_process_cons(bprb_func_process& pro)
{
  //this process takes one input:
  // input(0): the vil_image_view_base_sptr
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");

  //this process has one output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_rgbi_to_grey_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_rgbi_to_grey_process: The number of inputs should be 1" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  auto *out_img=new vil_image_view<unsigned char>(img->ni(),img->nj());

  vil_convert_planes_to_grey<unsigned char,unsigned char>(*(img.as_pointer()),*out_img);
  vil_image_view_base_sptr out_img_ptr=out_img;

  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_ptr);
  return true;
}
