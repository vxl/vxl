// This is brl/bseg/bsgm/pro/processes/bsgm_matching_stereo_process.cxx
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file
//      Process to run semi-global matching stereo algorithm for two rectified images

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <bsgm/bsgm_disparity_estimator.h>
#include <bsgm/bsgm_multiscale_disparity_estimator.h>

//: Take two rectified images, generate their disparity map calculated using semi-global matching stereo algorithm
namespace bsgm_matching_stereo_process_globals
{
  constexpr unsigned n_inputs_ = 9;
  constexpr unsigned n_outputs_ = 2;
}

bool bsgm_matching_stereo_process_cons(bprb_func_process& pro)
{
  using namespace bsgm_matching_stereo_process_globals;
  // process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input rectified image 1
  input_types_[1] = "vil_image_view_base_sptr";  // input rectified image 2
  input_types_[2] = "int";                       // minimum disparity
  input_types_[3] = "int";                       // disparity range
  input_types_[4] = "int";                       // number of active disparities.  If num_active_disparities is less than num_disparities,
                                                 // the process will run the multi-scale SGM first on quarter-res images using num_disparities/4 disparities
  input_types_[5] = "int";                       // option to choose interpolation type:
                                                 // 0 -- no error checking;  1-- black-flagged error pixels; 2 -- interpolated error pixels
  input_types_[6] = "int";                       // the mode parameter to specify how to use disparity estimate from coarse scale.  0 -- single median;
                                                 // 1 -- block wise disparity;  2 -- entire disparity image from coarse scale
  input_types_[7] = "vcl_string";                // output text file to store the matched disparities per pixel
  input_types_[8] = "unsigned";                       // threshold that treats pixel as shadow pixels

  // process takes 2 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // output disparity image
  output_types_[1] = "vil_image_view_base_sptr";  // scaled disparity image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bsgm_matching_stereo_process(bprb_func_process& pro)
{
  using namespace bsgm_matching_stereo_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong inputs!!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr img_ref_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr img_tgr_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  int min_disparity = pro.get_input<int>(in_i++);
  int num_disparity = pro.get_input<int>(in_i++);
  int num_active_disparities = pro.get_input<int>(in_i++);
  int error_check_mode = pro.get_input<int>(in_i++);
  int multi_scale_mode = pro.get_input<int>(in_i++);
  std::string out_disparity_txt = pro.get_input<std::string>(in_i++);
  auto shadow_thresh = pro.get_input<unsigned>(in_i++);

  // load image
  auto* img_ref = dynamic_cast<vil_image_view<vxl_byte>*>(img_ref_sptr.ptr());
  if (!img_ref) {
    std::cerr << pro.name() << ": The input reference image pixel format: " << img_ref_sptr->pixel_format() << " is not supported!" << std::endl;
    return false;
  }
  auto* img_tgr = dynamic_cast<vil_image_view<vxl_byte>*>(img_tgr_sptr.ptr());
  if (!img_tgr) {
    std::cerr << pro.name() << ": The input reference image pixel format: " << img_tgr_sptr->pixel_format() << " is not supported!" << std::endl;
    return false;
  }
  vil_image_view<vxl_byte> img_right = vil_convert_to_grey_using_rgb_weighting(img_ref);
  vil_image_view<vxl_byte> img_left = vil_convert_to_grey_using_rgb_weighting(img_tgr);
  int img_width = img_right.ni(), img_height = img_right.nj();

  // setup sgm parameters
  bsgm_disparity_estimator_params params;
  params.use_16_directions = false;
  params.use_gradient_weighted_smoothing = true;
  params.census_weight = 0.3;
  params.xgrad_weight = 0.7;
  params.census_rad = 2;
  params.print_timing = true;
  params.error_check_mode = error_check_mode;
  params.shadow_thresh = static_cast<vxl_byte>(shadow_thresh);

  // compute invalid map
  vil_image_view<bool> invalid_right;
  compute_invalid_map( img_right, img_left,
    invalid_right, min_disparity, num_disparity );

  //vil_save(invalid_right, "D:/temp/sgm/mountain_pair/invalid_right.tif");
  // Flip the sign of the disparities to match OpenCV implementation. Set the
  // invalid disparity to one less than the min value, before and after flip.
  float invalid_disp = min_disparity - 1.0f;
  float min_disparity_inv =  -( min_disparity + num_disparity - 1 );
  float invalid_disp_inv = min_disparity_inv - 1.0f;
  //vil_image_view<float> disp_right;

  // execute
  vil_image_view<float> disp_r;
  if (num_active_disparities >= num_disparity )  // run single-scale SGM if all disparities are active
  {
    bsgm_disparity_estimator sgm(
      params, img_width, img_height, num_disparity );

    vil_image_view<int> min_disp_img( img_width, img_height );
    min_disp_img.fill( min_disparity_inv );

    if (!sgm.compute( img_right, img_left, invalid_right,
        min_disp_img, invalid_disp_inv, disp_r ) ) {
      std::cerr << pro.name() << ": single-scale SGM failed!!\n";
      return false;
    }
  }
  else // otherwise run multi-scale to find the valid disparity range
  {
    bsgm_multiscale_disparity_estimator sgm(
      params, img_width, img_height, num_disparity, num_active_disparities );
    if (multi_scale_mode < 0 || multi_scale_mode > 2) {
      std::cerr << pro.name() << "Unknown multi_scale_mode: " << multi_scale_mode << ", only 0, 1, 2 are available!\n";
      return false;
    }
    std::cout << "Multi-scale SGM matching with mode " << multi_scale_mode << std::endl;
    if (!sgm.compute( img_right, img_left, invalid_right,
        min_disparity_inv, invalid_disp_inv, multi_scale_mode, disp_r ) ){
      std::cerr << pro.name() << ": multi-scale SGM failed!!\n";
      return false;
    }
  }

  // Flip the sign of the disparities to match OpenCV implementation.
  bsgm_invert_disparities( disp_r, invalid_disp_inv, invalid_disp );

  // convert a text file to save all disparity value
  std::ofstream ofs(out_disparity_txt.c_str());
  std::cout << "disparity size row: " << disp_r.nj() << ", cols: " << disp_r.ni() << std::endl;
  ofs << disp_r.nj() << " " << disp_r.ni() << std::endl;
  for (unsigned row_id = 0; row_id < disp_r.nj(); row_id++) {
    for (unsigned col_id = 0; col_id < disp_r.ni(); col_id++) {
      ofs << disp_r(col_id, row_id) << " ";
    }
    ofs << std::endl;
  }
  ofs << std::endl;
  ofs.close();

  // convert floating point image to byte for visualization/saving
  vil_image_view<vxl_byte> disp_r_8u;
  vil_convert_stretch_range_limited(disp_r, disp_r_8u, (float)min_disparity-1.0f, (float)(min_disparity+num_disparity-1));

  // generate output
  vil_image_view_base_sptr out_disp_sptr = new vil_image_view<float>(disp_r);
  vil_image_view_base_sptr out_disp_8u_sptr = new vil_image_view<vxl_byte>(disp_r_8u);

  pro.set_output_val<vil_image_view_base_sptr>(0, out_disp_sptr);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_disp_8u_sptr);
  return true;
}
