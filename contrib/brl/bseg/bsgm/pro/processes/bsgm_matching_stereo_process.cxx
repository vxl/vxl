// This is brl/bseg/bsgm/pro/processes/bsgm_matching_stereo_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//      Process to run semi-global matching stereo algorithm for two rectified images
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <bprb/bprb_parameters.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <bsgm/bsgm_disparity_estimator.h>

//: Take two rectified images, generate their disparity map calculated using semi-global matching stereo algorithm
namespace bsgm_matching_stereo_process_globals
{
  const unsigned n_inputs_ = 7;
  const unsigned n_outputs_ = 2;
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
  input_types_[5] = "int";                       // option to choose interpolation type: 0 -- no error checking;  1-- black-flagged error pixels; 2 -- interpolated error pixels
  input_types_[6] = "vcl_string";                // output text file to store the matched disparities per pixel

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
  int min_disparity                     = pro.get_input<int>(in_i++);
  int num_disparity                     = pro.get_input<int>(in_i++);
  int num_active_disparities            = pro.get_input<int>(in_i++);
  int error_check_mode                  = pro.get_input<int>(in_i++);
  std::string out_disparity_txt         = pro.get_input<std::string>(in_i++);

  // load image
  vil_image_view<vxl_byte>* img_ref = dynamic_cast<vil_image_view<vxl_byte>*>(img_ref_sptr.ptr());
  if (!img_ref) {
    std::cerr << pro.name() << ": The input reference image pixel format: " << img_ref_sptr->pixel_format() << " is not supported!" << std::endl;
    return false;
  }
  vil_image_view<vxl_byte>* img_tgr = dynamic_cast<vil_image_view<vxl_byte>*>(img_tgr_sptr.ptr());
  if (!img_tgr) {
    std::cerr << pro.name() << ": The input reference image pixel format: " << img_tgr_sptr->pixel_format() << " is not supported!" << std::endl;
    return false;
  }
  vil_image_view<vxl_byte> img_right = vil_convert_to_grey_using_rgb_weighting(img_ref);
  vil_image_view<vxl_byte> img_left  = vil_convert_to_grey_using_rgb_weighting(img_tgr);

  // setup sgm parameters
  bsgm_disparity_estimator_params params;
  params.use_16_directions = false;
  params.use_gradient_weighted_smoothing = true;
  params.census_weight = 0.3;
  params.xgrad_weight = 0.7;
  params.census_rad = 2;
  params.print_timing = true;
  params.error_check_mode = error_check_mode;

  // setup SGM
  bsgm_disparity_estimator sgm( params );

  // execute
  vil_image_view<float> disp_r;
  if (num_active_disparities >= num_disparity )  // run single-scale SGM if all disparities are active
  {
    if (!sgm.compute(img_right, img_left, disp_r, min_disparity, num_disparity) ) {
      std::cerr << pro.name() << ": single-scale SGM failed!!\n";
      return false;
    }
  }
  else                                           // otherwise run multi-scale to find the valid disparity range
  {
    if (!sgm.compute_multiscale(img_right, img_left, disp_r, min_disparity, num_disparity, num_active_disparities)) {
      std::cerr << pro.name() << ": multi-scale SGM failed!!\n";
      return false;
    }
  }
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
