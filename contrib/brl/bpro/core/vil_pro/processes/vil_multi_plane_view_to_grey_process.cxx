// This is brl/bpro/core/vil_pro/processes/vil_multi_plane_view_to_grey_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <bil/bil_convert_to_grey.h>
#include <vil/vil_image_resource.h>

//: Constructor
bool vil_multi_plane_view_to_grey_process_cons(bprb_func_process& pro)
{
  //this process takes one input:
  // input(0): the vil_image_view_base_sptr
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_resource_sptr");
  input_types.emplace_back("bool");

  //this process has one output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_resource_sptr");

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_multi_plane_view_to_grey_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_multi_plane_view_to_grey_process: The number of inputs should be 2" << std::endl;
    return false;
  }
  unsigned i=0;
  //Retrieve image from input
  vil_image_resource_sptr resc = pro.get_input<vil_image_resource_sptr>(i++);
  if(!resc){
    std::cout << "Null input image view" << std::endl;
    return false;
  }
  bool apply_mask = pro.get_input<bool>(i);
  std::cout << "INPUT IMAGE RESC(" << resc->ni() << ' ' << resc->nj() << ' ' << resc->nplanes() << ")" << std::endl;
  vil_pixel_format fmt = vil_pixel_format_component_format(resc->pixel_format());
  std:: cout << "INPUT Pixel Format " << fmt << std::endl;
  vil_image_resource_sptr grey_resc;
  bil_convert_resource_to_grey cnv;
  cnv(resc, grey_resc, apply_mask);
  pro.set_output_val<vil_image_resource_sptr>(0, grey_resc);
  return true;
}
