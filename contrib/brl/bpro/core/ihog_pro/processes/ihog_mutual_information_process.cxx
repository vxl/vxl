// This is brl/bpro/core/ihog_pro/processes/ihog_mutual_information_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for computing mutual information between two images

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <ihog/ihog_minimizer.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_minfo_cost_func.h>
#include <vil/vil_convert.h>


//: Init function
bool ihog_mutual_information_process_cons(bprb_func_process& pro)
{
  //this process takes three inputs and one output:
  //  0) image 0
  //  1) image 1
  //  2) mask

  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vil_image_view_base_sptr");
  std::vector<std::string> output_types;
  output_types.emplace_back("float");
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool ihog_mutual_information_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    std::cout << "ihog_mutual_information_process: The input number should be " << pro.input_types().size() << std::endl;
    return false;
  }
  // get the inputs
  vil_image_view<vxl_byte> img0_in(pro.get_input<vil_image_view_base_sptr>(0));
  vil_image_view<vxl_byte> img1_in(pro.get_input<vil_image_view_base_sptr>(1));

  vil_image_view_base_sptr mask_in = pro.get_input<vil_image_view_base_sptr>(2);

  unsigned ni = img0_in.ni(), nj = img0_in.nj();
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(img0_in,img0);

  if ((img1_in.ni() != ni) || (img1_in.nj() != nj)) {
    std::cerr << "ERROR: Images should have same dimensions\n";
    return false;
  }
  vil_image_view<float> img1(ni,nj);
  vil_convert_cast(img1_in, img1);

  if ((mask_in->ni() != ni) || (mask_in->nj() != nj)) {
    std::cerr << "ERROR: Mask should have same dimensions as images\n";
    return false;
  }

  vil_image_view<float> mask(ni,nj);
  if (auto* mask_byte = dynamic_cast<vil_image_view<vxl_byte>*>(mask_in.ptr())) {
    vil_convert_stretch_range_limited(*mask_byte, mask, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
  }
  else if (auto* mask_bool = dynamic_cast<vil_image_view<bool>*>(mask_in.ptr())) {
    vil_convert_cast(*mask_bool, mask);
  }
  else {
    std::cerr << "ERROR: unsupported mask type\n";
    return false;
  }

  int border = 2;
  ihog_world_roi roi(img0.ni()- 2*border,
                     img0.nj()- 2*border,
                     vgl_point_2d<double>(border,border));

  ihog_transform_2d init_xform;
  init_xform.set_translation_only(0,0);

  ihog_image<float> from_img(img1, ihog_transform_2d());
  ihog_image<float> to_img(img0, ihog_transform_2d());
  //ihog_image<float> mask1_img(mask1, ihog_transform_2d());
  ihog_image<float> mask_img(mask, ihog_transform_2d());

  ihog_minfo_cost_func cost_fun(to_img, from_img, mask_img, roi, init_xform);
  vnl_vector<double> x;
  init_xform.params(x);
  double minfo = cost_fun.f(x);

  pro.set_output_val<float>(0,(float)minfo);

  return true;
}
