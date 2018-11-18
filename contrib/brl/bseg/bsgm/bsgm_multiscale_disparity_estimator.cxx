// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

#include <iomanip>
#include <algorithm>

#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_resample_nearest.h>
#include <vil/vil_resample_bilin.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_gauss_reduce.h>

#include "bsgm_multiscale_disparity_estimator.h"

using namespace std;


//----------------------------------------------------------------------------
int bsgm_compute_median_of_image(
  const vil_image_view<float>& img,
  const vil_image_view<bool>& invalid,
  int start_x,
  int end_x,
  int start_y,
  int end_y,
  int min_img_val,
  int num_img_vals,
  float invalid_disparity )
{
  // Build a histogram
  std::vector<long int> hist( num_img_vals, 0 );
  long int hist_count = 0;
  for( int y = start_y; y < end_y; y++ ){
    for( int x = start_x; x < end_x; x++ ){
      if( (img(x,y) == invalid_disparity) || invalid(x,y) ) continue;

      int sample = (int)( img(x,y) - min_img_val );
      if( sample < 0 || sample >= num_img_vals ) continue;
      hist[ sample ]++;
      hist_count++;
    }
  }

  // No valid pixels
  if( hist_count == 0 ) return 0;

  // Find the median of the histogram
  auto med_count = (long int)( hist_count*0.5 );
  hist_count = 0;
  for( int s = 0; s < num_img_vals; s++ ){
    hist_count += hist[s];
    if( hist_count > med_count )
      return s + min_img_val;
  }

  // Impossible, only to prevent compiler warnings
  return 0;
}


//----------------------------------------------------------------------------
bsgm_multiscale_disparity_estimator::bsgm_multiscale_disparity_estimator(
  const bsgm_disparity_estimator_params& params,
  int img_width,
  int img_height,
  int num_disparities,
  int num_active_disparities,
  int downscale_exp ) :
    fine_w_( img_width ),
    fine_h_( img_height ),
    num_fine_disparities_( num_disparities ),
    num_active_disparities_( num_active_disparities )
{
  if( downscale_exp <= 0 || downscale_exp > 4 ) downscale_exponent_ = 2;
  else downscale_exponent_ = downscale_exp;

  downscale_factor_ = std::pow( 2, downscale_exponent_ );

  // Calculate size of downsampled images
  coarse_w_ = img_width;
  coarse_h_ = img_height;
  for( int s = 0; s < downscale_exponent_; s++ ){
    coarse_w_ = (coarse_w_+1)/2;
    coarse_h_ = (coarse_h_+1)/2;
  }

  // Compute coarse disparities
  num_coarse_disparities_ = num_disparities / downscale_factor_;

  // Set up the single-scale SGMs
  coarse_de_ = new bsgm_disparity_estimator(
    params, coarse_w_, coarse_h_, num_coarse_disparities_ );
  fine_de_ = new bsgm_disparity_estimator(
    params, fine_w_, fine_h_, num_active_disparities );
}


//----------------------------------------------------------------------------
bsgm_multiscale_disparity_estimator::~bsgm_multiscale_disparity_estimator()
{
  delete coarse_de_;
  delete fine_de_;
}


//----------------------------------------------------------------------------
bool
bsgm_multiscale_disparity_estimator::compute(
  const vil_image_view<vxl_byte>& img_tar,
  const vil_image_view<vxl_byte>& img_ref,
  const vil_image_view<bool>& invalid_tar,
  int min_disp,
  float invalid_disp,
  int const& multi_scale_mode,
  vil_image_view<float>& disp_tar)
{
  //int multi_scale_mode = 1;

  // Size of block used in mode 1
  int block_size = 100;
  disp_tar.set_size( fine_w_, fine_h_ );

  // Validate images.
  if( img_tar.ni() != fine_w_ || img_tar.nj() != fine_h_ ||
      img_ref.ni() != fine_w_ || img_ref.nj() != fine_h_ ||
      invalid_tar.ni() != fine_w_ || invalid_tar.nj() != fine_h_ )
    return false;

  // Downsample the images
  vil_image_view<vxl_byte> img_t_coarse, img_r_coarse,
    temp_t_coarse, temp_r_coarse, working;

  vil_gauss_reduce( img_tar, img_t_coarse, working );
  vil_gauss_reduce( img_ref, img_r_coarse, working );

  for( int s = 1; s < downscale_exponent_; s++ ){
    vil_gauss_reduce( img_t_coarse, temp_t_coarse, working );
    img_t_coarse.deep_copy( temp_t_coarse );
    vil_gauss_reduce( img_r_coarse, temp_r_coarse, working );
    img_r_coarse.deep_copy( temp_r_coarse );
  }

  // Downsample the invalid image and dilate to handle borders
  vil_image_view<bool> invalid_t_resamp( coarse_w_, coarse_h_ );
  vil_resample_nearest( invalid_tar, invalid_t_resamp, coarse_w_, coarse_h_ );
  vil_image_view<bool> invalid_t_coarse( coarse_w_, coarse_h_ );
  vil_structuring_element se; se.set_to_disk( 3.0 );
  vil_binary_dilate( invalid_t_resamp, invalid_t_coarse, se );

  vil_image_view<int> min_disp_img_coarse( coarse_w_, coarse_h_ );
  vil_image_view<int> min_disp_img_fine( fine_w_, fine_h_ );

  // Setup trivial coarse-scale min disparity image
  int min_disp_coarse = min_disp / downscale_factor_;
  min_disp_img_coarse.fill( min_disp_coarse );

  float invalid_disp_coarse = invalid_disp / downscale_factor_;

  // Run SGM on coarse scale images
  vil_image_view<float> disp_t_coarse;
  if( !coarse_de_->compute( img_t_coarse, img_r_coarse, invalid_t_coarse,
      min_disp_img_coarse, invalid_disp_coarse, disp_t_coarse ) )
    return false;

  //DEBUG
  //vil_save( disp_t_coarse, "D:/temp/sgm/mountain_pair/disp_t_coarse.tif");


  // Mode 0 uses a constant min disparity for all pixels
  if( multi_scale_mode == 0 ){

    // Compute the median disparity for the
    int med_ds = bsgm_compute_median_of_image(
      disp_t_coarse, invalid_t_coarse, 0, coarse_w_, 0, coarse_h_,
      min_disp_coarse, num_coarse_disparities_, invalid_disp_coarse );

    // Setup fine-scale min disparity image
    min_disp_img_fine.fill(
      med_ds*downscale_factor_ - num_active_disparities_/2 );

  // Mode 1 uses a fixed min disparity in large block regions
  } else if( multi_scale_mode == 1 ) {

    int coarse_block_size = block_size / downscale_factor_;

    for( int start_y = 0; start_y < coarse_h_; start_y+=coarse_block_size ){
      for( int start_x = 0; start_x < coarse_w_; start_x+=coarse_block_size ){

        int end_x = min( coarse_w_, start_x+coarse_block_size );
        int end_y = min( coarse_h_, start_y+coarse_block_size );

        // Compute median of the box region
        int med_ds = bsgm_compute_median_of_image( disp_t_coarse,
          invalid_t_coarse, start_x, end_x, start_y, end_y,
          min_disp_coarse, num_coarse_disparities_, invalid_disp_coarse );

        // Set the min (fine-scale) disparity for each pixel in the box to a
        // value centered on this median.
        for( int by = start_y; by < end_y; by++ ){
          for( int bx = start_x; bx < end_x; bx++ ){
            min_disp_img_coarse(bx,by) =
              med_ds*downscale_factor_ - num_active_disparities_/2;
          }
        }
      }
    }

    // Upscale to full-res
    vil_resample_bilin(
      min_disp_img_coarse, min_disp_img_fine, fine_w_, fine_h_ );

  // Mode 2 uses per pixel min disparity based on coarse disparity
  } else {

    // Set the min (fine-scale) disparity for each pixel to a value
    // value centered on this median.
    for( int y = 0; y < coarse_h_; y++ ){
      for( int x = 0; x < coarse_w_; x++ ){
        min_disp_img_coarse(x,y) =
         disp_t_coarse(x,y)*downscale_factor_ - num_active_disparities_/2;
      }
    }

    // Upscale to full-res
    vil_resample_bilin(
      min_disp_img_coarse, min_disp_img_fine, fine_w_, fine_h_ );
  }

  // Run fine-scale SGM
  if( !fine_de_->compute( img_tar, img_ref, invalid_tar,
      min_disp_img_fine, invalid_disp, disp_tar ) )
    return false;

  //fine_de_->write_cost_debug_imgs( std::string("C:/data/results"), true );

  return true;
}
