// This is brl/bpro/core/brip_pro/processes/brip_phase_correlation_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  process to compute an image translation via phase correlation to align the two images
// \verbatim
//  Modifications
//
// \endverbatim

#include <brip/brip_phase_correlation.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
//: global variables
namespace brip_phase_correlation_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 4;
}

//: constructor
bool brip_phase_correlation_process_cons(bprb_func_process& pro)
{
  using namespace brip_phase_correlation_process_globals;
  // inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input image 1
  input_types_[1] = "vil_image_view_base_sptr";  // input image 2
  input_types_[2] = "float";                    // gauss sigma
  input_types_[3] = "float";                    // peak_radius
  input_types_[4] = "float";                  // alpha
  // outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "float";  // tu
  output_types_[1] = "float";  // tv
  output_types_[2] = "float";  // confidence
  output_types_[3] = "vil_image_view_base_sptr";  // correlation output

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: execute the process
bool brip_phase_correlation_process(bprb_func_process& pro)
{
  using namespace brip_phase_correlation_process_globals;

  // sanity check
  if (!pro.verify_inputs()) {
    std::cout << pro.name() << ": Wrong Inputs!!!" << std::endl;
    return false;
  }

  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_ptr_b = pro.get_input<vil_image_view_base_sptr>(in_i++);
  auto gauss_sigma = pro.get_input<float>(in_i++);
  auto peak_radius = pro.get_input<float>(in_i++);
  auto alpha = pro.get_input<float>(in_i++);

  // cast to float images
  vil_image_view<float> image_a = *(vil_convert_cast(float(), img_ptr_a));
  vil_image_view<float> image_b = *(vil_convert_cast(float(), img_ptr_b));

  // replace NAN values with proper fill value (0.0f)
  for (unsigned i = 0; i < image_a.ni(); i++)
    for (unsigned j = 0; j < image_a.nj(); j++)
      if (image_a(i,j) != image_a(i,j)) // test for std::numeric_limits<float>::quiet_NaN())
        image_a(i,j) = 0.0f;

  for (unsigned i = 0; i < image_b.ni(); i++)
    for (unsigned j = 0; j < image_b.nj(); j++)
      if (image_b(i,j) != image_b(i,j)) // test for std::numeric_limits<float>::quiet_NaN())
        image_b(i,j) = 0.0f;
  //vil_save(image_a, "C:/projects/SatTel/Kandahar/scene_60/image_a.tif");
  //vil_save(image_b, "C:/projects/SatTel/Kandahar/scene_60/image_b.tif");
  brip_phase_correlation bpco(image_a, image_b, gauss_sigma, peak_radius, alpha);
  bool good = bpco.compute();
  if(!good) {
    std::cout << "In brip_phase_correlation_process -- compute() failed\n";
    return false;
  }

  vil_image_view<float> ortho_corr = bpco.correlation_array();
  float tu , tv, conf;
  if(!bpco.translation(tu, tv, conf)) {
    std::cout << "In brip_phase_correlation_process -- registration failed\n";
    return false;
  }
  pro.set_output_val<float>(0, tu);
  pro.set_output_val<float>(1, tv);
  pro.set_output_val<float>(2, conf);
  pro.set_output_val<vil_image_view_base_sptr>(3, new vil_image_view<float>(ortho_corr));
  return true;
}
