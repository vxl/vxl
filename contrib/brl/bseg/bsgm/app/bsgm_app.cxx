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

#include <bsgm\bsgm_disparity_estimator.h>


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
  if( argc != 6 ){
    std::cout<<"Usage : bsgm_app.exe target_img ref_img disp_img min_disparity num_disparities\n";
    return -1;
  }
    
  // Read inputs
  std::string right_name(argv[1]);
  std::string left_name(argv[2]);
  std::string disp_name(argv[3]);
  int min_disparity = atoi(argv[4]);
  int num_disparities = atoi(argv[5]);

  // Load images
  vil_image_view<vxl_byte> img_right = 
    vil_convert_to_grey_using_rgb_weighting( vil_load( right_name.c_str() ) );
  vil_image_view<vxl_byte> img_left = 
    vil_convert_to_grey_using_rgb_weighting( vil_load( left_name.c_str() ) );

  // Setup SGM
  params.min_disparity = min_disparity;
  params.num_disparities = num_disparities;

  bsgm_disparity_estimator sgm( params );

  // Run SGM
  vil_image_view<float> disp_r;
  if( !sgm.compute( img_right, img_left, disp_r ) ){
    std::cerr << "SGM failed\n";
    return 1;
  }

  // Convert floating point image to byte for saving
  vil_image_view<vxl_byte> disp_r_8u;
  vil_convert_stretch_range_limited( disp_r, disp_r_8u, (float)min_disparity-1,
    (float)( min_disparity+num_disparities-1 ) );
  vil_save( disp_r_8u, disp_name.c_str() );

  // Debugging
  //sgm.write_cost_debug_imgs( std::string("C:/data/results"), true );

  return 0;
};
