// This is brl/bpro/core/sdet_pro/processes/sdet_texture_classifier_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_atmospheric_image_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>

//: initialize input and output types
bool sdet_texture_unsupervised_classifier_process_cons(bprb_func_process& pro)
{
  // process takes 3 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //classifier
  if (!pro.set_input_types(input_types))
    return false;

  // process has 2 outputs:
  // output[0]: output texture color image
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // rgb color image
  output_types.emplace_back("vil_image_view_base_sptr");  // id image
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_unsupervised_classifier_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr dict = pro.get_input<sdet_texture_classifier_sptr>(0);
  std::cout << "max filter radius in dictionary: " << dict->max_filter_radius() << std::endl;
  int invalid = dict->max_filter_radius();
  unsigned ntextons = dict->get_number_of_textons();
  std::cout << " testing using the dictionary with the number of textons: " << ntextons << "\n categories:\n";

  // create random colors for textons
  vnl_random rng(10);  // always start with same seed to get same colors
  std::vector<vil_rgb<vxl_byte> > colors;
  for (unsigned kk = 0; kk < ntextons; kk++) {
    unsigned char r = rng.lrand32(0,255);
    unsigned char g = rng.lrand32(0,255);
    unsigned char b = rng.lrand32(0,255);
    colors.emplace_back(r,g,b);
  }

  // assumes the filter bank is computed at the dictionary
  unsigned ni = dict->filter_responses().ni();
  unsigned nj = dict->filter_responses().nj();
  std::cout << "filter bank already computed at the dictionary, ni: " << ni << " nj: " << nj << std::endl;

  vil_image_view<vil_rgb<vxl_byte> > out_rgb(ni, nj);
  out_rgb.fill(vil_rgb<vxl_byte>(0,0,0));

  vil_image_view<int> texton_img(ni, nj);
  texton_img.fill(-1);
  dict->compute_textons_of_pixels(texton_img);
  std::cout << " computed textons of pixels..!\n"; std::cout.flush();

  for (int i = invalid; i < (int)ni-invalid; i++) {
    for (int j = invalid; j < (int)nj-invalid; j++) {
      int indx = texton_img(i, j);
      if (indx < 0)
        continue;
      out_rgb(i,j) = colors[indx];
    }
  }

  // return the output image
  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<vil_rgb<vxl_byte> >(out_rgb));
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<int >(texton_img));
  return true;
}
