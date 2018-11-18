// This is brl/bpro/core/ihog_pro/processes/ihog_compute_mi_cost_surface_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to register two images by finding the best translational mapping.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <ihog/ihog_minimizer.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_minfo_cost_func.h>
#include <vil/vil_convert.h>


//: Init function
bool ihog_compute_mi_cost_surface_process_cons(bprb_func_process& pro)
{
  //this process takes four inputs and one output:
  //  0) image 0
  //  1) image 1
  //  2) mask
  //  3) radius
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("vil_image_view_base_sptr");
  input_types.emplace_back("int");
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");

  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool ihog_compute_mi_cost_surface_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    std::cout << "ihog_compute_mi_cost_surface_process: The input number should be " << pro.input_types().size() << std::endl;
    return false;
  }
  // get the inputs
  vil_image_view<vxl_byte> img_0(pro.get_input<vil_image_view_base_sptr>(0));
  vil_image_view<vxl_byte> img_0_mask(pro.get_input<vil_image_view_base_sptr>(1));
  vil_image_view<vxl_byte> img_1(pro.get_input<vil_image_view_base_sptr>(2));
  int radius = pro.get_input<int>(3);

  unsigned ni = img_0.ni(), nj = img_0.nj();
  vil_image_view<float> img0(ni,nj);
  vil_convert_cast(img_0,img0);

  vil_image_view<float> img1(img_1.ni(),img_1.nj());
  vil_convert_cast(img_1, img1);

  vil_image_view<float> mask0(img_0_mask.ni(), img_0_mask.nj());
  vil_convert_stretch_range_limited(img_0_mask, mask0, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);

  vil_image_view<float> mask1(img_1.ni(),img_1.nj());
  mask1.fill(1.0f);

  int border = 2;
  ihog_world_roi roi(img0.ni()- 2*border,
                     img0.nj()- 2*border,
                     vgl_point_2d<double>(border,border));

  ihog_transform_2d init_xform;
  init_xform.set_translation_only(0,0);
  ihog_image<float> from_img(img1, init_xform);
  //std::cout << "init_xform.form = " << from_img.world2im().form() << std::endl;
  //return true;
  ihog_image<float> to_img(img0, ihog_transform_2d());
  ihog_image<float> mask1_img(mask1, ihog_transform_2d());
  ihog_image<float> mask0_img(mask0, ihog_transform_2d());

  //ihog_minfo_cost_func cost_fun(to_img, from_img, mask0_img, mask1_img, roi, init_xform);
  //ihog_minfo_cost_func cost_fun(to_img, from_img, roi, init_xform);
  ihog_minfo_cost_func cost_fun(to_img, from_img, mask0_img, roi, init_xform);

  float step = 1.0f;
  int half_n_steps = radius;
  int n_steps = 2*half_n_steps + 1;
  auto *cost_map = new vil_image_view<float>(n_steps,n_steps);
  vil_image_view_base_sptr cost_map_sptr(cost_map);

  for (int i=0; i<n_steps; ++i) {
    std::cout << "-------------------------------------  i = " << i << std::endl;
    for (int j=0; j<n_steps; ++j) {
      float offset_x = (j - half_n_steps)*step;
      float offset_y = (i - half_n_steps)*step;
      ihog_transform_2d xform;
      xform.set_translation_only(offset_x,offset_y);
      vnl_vector<double> x;
      xform.params(x);
      //std::cout << "x = " << x << std::endl;
      auto minfo = float(cost_fun.f(x));
      (*cost_map)(i,j) = minfo;
      std::cout << "minfo(" << offset_x << ", " << offset_y << ") = " << minfo << " (" << i << ", " << j << ')' << std::endl;
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(0,cost_map_sptr);

  return true;
}
