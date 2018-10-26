// This is brl/bpro/core/vil_pro/processes/vil_median_filter_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_median.h>

//: Constructor
bool vil_median_filter_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  //: original image
  input_types.emplace_back("int");  //: square mask size
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
bool vil_median_filter_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 2) {
    std::cout << "vil_median_filter_process: The input number should be 3" << std::endl;
    return false;
  }

  unsigned i=0;
  //Retrieve image from input
  vil_image_view_base_sptr image = pro.get_input<vil_image_view_base_sptr>(i++);
  int medfilt_halfsize = pro.get_input<int>(i++);

  //int medfilt_halfsize = 1;
  vil_image_view<float> orig_img(image);
  vil_image_view<float> out_img(image->ni(), image->nj());
  out_img.fill(0.0f);

  std::vector<int> strel_vec_i, strel_vec_j;
  for (int i=-medfilt_halfsize; i <= medfilt_halfsize; ++i)
    for (int j=-medfilt_halfsize; j <= medfilt_halfsize; ++j) {
      strel_vec_i.push_back(i);
      strel_vec_j.push_back(j); }

  vil_structuring_element strel(strel_vec_i,strel_vec_j);
  vil_median(orig_img,out_img,strel);

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(out_img));
  return true;
}
