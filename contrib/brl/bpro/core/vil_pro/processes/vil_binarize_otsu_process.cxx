// This is brl/bpro/core/vil_pro/processes/vil_binarize_otsu_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Binarize an image using Otsu Thresholding (a threshold computed from the appearance distribution of the image)

#include <vil/vil_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <bsta/bsta_otsu_threshold.h>

//: Constructor
bool vil_binarize_otsu_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the image
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("double");  // range
  input_types.emplace_back("int");     // number of bins
  input_types.emplace_back("int");     // margin
  input_types.emplace_back("double");  // pixel vals that will be ignored

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // binary RGB image with black and white pixels
  output_types.emplace_back("double");  // binary RGB image with black and white pixels
  return pro.set_input_types(input_types)
     &&  pro.set_output_types(output_types);
}

//: Execute the process
bool vil_binarize_otsu_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "vil_binarize_otsu_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the input
  unsigned i=0;
  vil_image_view_base_sptr img_ptr_a = pro.get_input<vil_image_view_base_sptr>(i++);
  auto range = pro.get_input<double>(i++);
  int bins = pro.get_input<int>(i++);
  int margin = pro.get_input<int>(i++);
  auto invalid_pix = pro.get_input<double>(i++);

  //double range = 0.1;
  //int bins = 10000;

  // for now assume input is a float image
  vil_image_view<float> view = *(vil_convert_cast(float(), img_ptr_a));
  int ni = view.ni();
  int nj = view.nj();

  //determine the range first
  range = 0;
  for (unsigned j = margin; j < nj-margin; ++j) {
    for (unsigned i = margin; i < ni-margin; ++i) {
      if (view(i,j) > range)
        range = view(i,j);
    }
  }
  std::cout << "Otsu Thresholding process, range: " << range << '\n';
  // binarize using otsu thresholding
  bsta_histogram<double> h(range, bins);
  for (unsigned j = margin; j < nj-margin; ++j) {
    for (unsigned i = margin; i < ni-margin; ++i) {
      if (std::fabs(view(i,j) - invalid_pix) > 1E-3)
        h.upcount(view(i,j), 1.0);
    }
  }
  bsta_otsu_threshold<double> ot(h);
  double dt = ot.threshold();
  std::cout << "Otsu Threshold " << dt << '\n';

  auto* out_bin_img = new vil_image_view<vil_rgb<vxl_byte> >(ni, nj);
  out_bin_img->fill(vil_rgb<vxl_byte>(0,0,0));
  for (unsigned j = 0; j < nj; ++j) {
    for (unsigned i = 0; i < ni; ++i) {
      if (view(i,j) >= dt)
        (*out_bin_img)(i,j) = vil_rgb<vxl_byte>(255,255,255);
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0, out_bin_img);
  pro.set_output_val<double>(1, dt);
  return true;
}
