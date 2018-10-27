// This is brl/bpro/core/vil_pro/processes/vil_crop_image_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_new.h>

//: Constructor
bool vil_crop_image_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("unsigned");
  input_types.emplace_back("unsigned");
  input_types.emplace_back("unsigned");
  input_types.emplace_back("unsigned");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool vil_crop_image_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 5) {
    std::cout << "vil_crop_image_process: The input number should be 5" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_resource_sptr image_ptr = vil_new_image_resource_of_view(*image);

  //Retrieve limits
  auto i0= pro.get_input<unsigned>(i++);
  auto j0= pro.get_input<unsigned>(i++);
  auto ni= pro.get_input<unsigned>(i++);
  auto nj= pro.get_input<unsigned>(i++);

  vil_image_resource_sptr out_img = vil_crop(image_ptr, i0, ni, j0, nj);
  vil_image_view_base_sptr out_sptr = vil_new_image_view_base_sptr(*(out_img->get_view()));

  pro.set_output_val<vil_image_view_base_sptr>(0, out_sptr);
  return true;
}

//: A crop process that takes vil_image_resource as input to crop the image
namespace vil_crop_image_res_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}

bool vil_crop_image_res_process_cons(bprb_func_process& pro)
{
  using namespace vil_crop_image_res_process_globals;
  std::vector<std::string> input_types(n_inputs_);
  input_types[0] = "vil_image_resource_sptr"; // image resource that require cropping
  input_types[1] = "unsigned";
  input_types[2] = "unsigned";
  input_types[3] = "unsigned";
  input_types[4] = "unsigned";

  std::vector<std::string> output_types(n_outputs_);
  output_types[0] = "vil_image_view_base_sptr";
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

//: Execute the process
bool vil_crop_image_res_process(bprb_func_process& pro)
{
  using namespace vil_crop_image_res_process_globals;
  // sanity check
  if (pro.n_inputs() != n_inputs_) {
    std::cout << pro.name() << ": The input number should be 5" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vil_image_resource_sptr img_res_sptr = pro.get_input<vil_image_resource_sptr>(i++);
  auto i0 = pro.get_input<unsigned>(i++);
  auto j0 = pro.get_input<unsigned>(i++);
  auto ni = pro.get_input<unsigned>(i++);
  auto nj = pro.get_input<unsigned>(i++);

  vil_image_resource_sptr out_img = vil_crop(img_res_sptr, i0, ni, j0, nj);
  vil_image_view_base_sptr out_sptr = vil_new_image_view_base_sptr(*(out_img->get_view()));

  pro.set_output_val<vil_image_view_base_sptr>(0, out_sptr);
  return true;
}
