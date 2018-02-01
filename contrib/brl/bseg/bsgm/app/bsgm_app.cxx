#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <bsgm/bsgm_disparity_estimator.h>
#include <bsgm/bsgm_multiscale_disparity_estimator.h>


int main(int argc,char * argv[])
{
  // Hardcoded params
  bsgm_disparity_estimator_params params;
  params.use_16_directions = false;
  params.use_gradient_weighted_smoothing = true;
  params.census_weight = 0.3;
  params.xgrad_weight = 0.7;
  params.census_rad = 2;
  params.print_timing = true;

  // Usage information
  if( argc != 8 && argc != 9 ){
    std::cout << "Usage : bsgm_app.exe target_img ref_img disp_img "
      << "min_disparity num_disparities num_active_disparities "
      << "multi_scale_mode error_check_mode\n";
    return -1;
  }

  // Read inputs
  std::string right_name(argv[1]);
  std::string left_name(argv[2]);
  std::string disp_name(argv[3]);
  int min_disparity = atoi(argv[4]);
  int num_disparities = atoi(argv[5]);
  int num_active_disparities = atoi(argv[6]);
  int multi_scale_mode = atoi(argv[7]);
  params.error_check_mode = (argc==8) ? 1 : atoi(argv[8]);
  params.shadow_thresh = 20;

  // Load images
  vil_image_view<vxl_byte> img_right =
    vil_convert_to_grey_using_rgb_weighting( vil_load( right_name.c_str() ) );
  vil_image_view<vxl_byte> img_left =
    vil_convert_to_grey_using_rgb_weighting( vil_load( left_name.c_str() ) );
  int img_width = img_right.ni(), img_height = img_right.nj();

  // Check images
  if( !(img_width > 0 && img_height > 0) ){
    std::cerr << "Failed to load images\n";
    return 1;
  }

  // Compute invalid map
  vil_image_view<bool> invalid_right;
  compute_invalid_map( img_right, img_left,
    invalid_right, min_disparity, num_disparities );

  // Flip the sign of the disparities to match OpenCV implementation. Set the
  // invalid disparity to one less than the min value, befor and after flip.
  float invalid_disp = min_disparity - 1.0f;
  float min_disparity_inv =  -( min_disparity + num_disparities - 1 );
  float invalid_disp_inv = min_disparity_inv - 1.0f;

  vil_image_view<float> disp_right;

  // Run single-scale SGM if all disparities are active
  if( num_active_disparities >= num_disparities ){

    bsgm_disparity_estimator sgm(
      params, img_width, img_height, num_disparities );

    vil_image_view<int> min_disp_img( img_width, img_height );
    min_disp_img.fill( min_disparity_inv );

    if( !sgm.compute( img_right, img_left, invalid_right,
        min_disp_img, invalid_disp_inv, disp_right ) ){
      std::cerr << "SGM failed\n";
      return 1;
    }

    // Debugging
    //sgm.write_cost_debug_imgs( std::string("C:/data/results"), true );

  // Otherwise run multi-scale to find the valid disparity range
  } else {

    bsgm_multiscale_disparity_estimator sgm(
      params, img_width, img_height, num_disparities, num_active_disparities );

    if( !sgm.compute( img_right, img_left, invalid_right,
        min_disparity_inv, invalid_disp_inv, multi_scale_mode, disp_right ) ){
      std::cerr << "SGM failed\n";
      return 1;
    }
  }

  // Flip the sign of the disparities to match OpenCV implementation.
  bsgm_invert_disparities( disp_right, invalid_disp_inv, invalid_disp );

  // Convert floating point image to byte for saving
  vil_image_view<vxl_byte> disp_r_8u;
  vil_convert_stretch_range_limited( disp_right, disp_r_8u,
    (float)min_disparity-1, (float)( min_disparity+num_disparities-1 ) );
  vil_save( disp_r_8u, disp_name.c_str() );

  return 0;
};
