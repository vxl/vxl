// This is brl/bpro/core/brip_pro/processes/brip_image_mutual_info_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  process to compute mutual information between two images
// \verbatim
//  Modifications
//
// \endverbatim

#include <brip/brip_mutual_info.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>

//: global variables
namespace brip_image_mutual_info_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}

//: constructor
bool brip_image_mutual_info_process_cons(bprb_func_process& pro)
{
  using namespace brip_image_mutual_info_process_globals;
  // inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input image 1
  input_types_[1] = "vil_image_view_base_sptr";  // input image 2
  input_types_[2] = "double";                    // histogram min value
  input_types_[3] = "double";                    // histogram max value
  input_types_[4] = "unsigned";                  // number of bins in histogram
  // outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";  // mutual information between two input images

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: execute the process
bool brip_image_mutual_info_process(bprb_func_process& pro)
{
  using namespace brip_image_mutual_info_process_globals;

  // sanity check
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": Wrong Inputs!!!" << std::endl;
    return false;
  }

  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_ptr_b = pro.get_input<vil_image_view_base_sptr>(in_i++);
  auto min = pro.get_input<double>(in_i++);
  auto max = pro.get_input<double>(in_i++);
  auto n_bins = pro.get_input<unsigned>(in_i++);

  // cast to float images
  vil_image_view<float> image_a = *(vil_convert_cast(float(), img_ptr_a));
  vil_image_view<float> image_b = *(vil_convert_cast(float(), img_ptr_b));

  double mutual_info = brip_mutual_info(image_a, image_b, min, max, n_bins);

  // output
  pro.set_output_val<double>(0, mutual_info);
  return true;
}
