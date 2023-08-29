// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef bsgm_multiscale_disparity_estimator_h_
#define bsgm_multiscale_disparity_estimator_h_

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <utility>

#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>
#include "vil/vil_resample_nearest.h"
#include "vil/vil_resample_bilin.h"
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_binary_dilate.h>
#include <vil/algo/vil_gauss_reduce.h>

#include "bsgm_disparity_estimator.h"

//:
// \file
// \brief A multi-scale implementation of SGM.
// \author Thomas Pollard
// \date June 7, 2016
//
//  Modifications
//   Jun, 2016 Yi Dong - add input parameter for function 'compute'
// \endverbatim


class bsgm_multiscale_disparity_estimator
{
 public:

  //: Construct from parameters. Coarse scale SGM will run on images
  // downsampled by 2^downscale_exponent
  bsgm_multiscale_disparity_estimator(
    const bsgm_disparity_estimator_params& params,
    int img_width,
    int img_height,
    int num_disparities,
    int num_active_disparities,
    vil_image_view<float> const& shadow_step_prob = vil_image_view<float>(),
    vgl_vector_2d<float> sun_dir_tar = vgl_vector_2d<float>(0.0f, 0.0f),
    int downscale_exponent = 2 );

  //: Destructor
  ~bsgm_multiscale_disparity_estimator();

  //: Run SGM twice, once at a lower resolution to determine the active
  // disparity range, and again at full-res using the reduced disparity range.
  // Should improve speed and quality with respect to single-scale approach if
  // tight disparity bounds are unknown.
  // mutli_scale_mode can be:
  // 0 Single min disparity used for entire image
  // 1 Single min disparity used within coarse image blocks
  // 2 Different min disparity used at each pixel
  template <class T>
  bool compute(
    const vil_image_view<T>& img_target,
    const vil_image_view<T>& img_ref,
    const vil_image_view<bool>& invalid_target,
    int min_disparity,
    float invalid_disparity,
    int const& multi_scale_mode,
    vil_image_view<float>& disp_target,
    float dynamic_range_factor = 1.0f,
    bool skip_error_check = false);

  //: Same as above, except compute disparity maps for both images and use a
  // full left-right consistency check to detect and fix errors in the
  // disparity maps.
  template <class T>
  bool compute_both(
    const vil_image_view<T>& img_target,
    const vil_image_view<T>& img_ref,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<bool>& invalid_ref,
    int min_disparity,
    float invalid_disparity,
    int const& multi_scale_mode,
    vil_image_view<float>& disp_target,
    vil_image_view<float>& disp_ref,
    float dynamic_range_factor = 1.0f);

  //: Write out the appearance or total cost volume as a set of images for
  // debugging
  void write_cost_debug_imgs(
    const std::string& out_dir,
    bool write_total_cost = false )
  {
    fine_de_->write_cost_debug_imgs(out_dir, write_total_cost);
  }
  vil_image_view<float> fill_1x1_holes(vil_image_view<float> const& img); 
 protected:
int bsgm_compute_median_of_image(
  const vil_image_view<float>& img,
  const vil_image_view<bool>& invalid,
  int start_x,
  int end_x,
  int start_y,
  int end_y,
  int min_img_val,
  int num_img_vals,
  float invalid_disparity );
  //: Size of image
  int coarse_w_, coarse_h_, fine_w_, fine_h_;

  //: Downscaling parameters where downscale_factor_ = 2^downscale_exponent_
  int downscale_exponent_;
  int downscale_factor_;

  int num_coarse_disparities_, num_fine_disparities_, num_active_disparities_;

  //: Single-scale SGMs for coarse and fine scales
  bsgm_disparity_estimator* coarse_de_;
  bsgm_disparity_estimator* fine_de_;
  bsgm_disparity_estimator_params params_;
  //illumination-related data
  vil_image_view<float> shadow_step_prob_;
  //down-sampled version
  vil_image_view<float> ss_coarse_;

  // sun direction for deprecated dp shadow bias method
  vgl_vector_2d<float> sun_dir_tar_ = vgl_vector_2d<float>(0.0, 0.0);
};
template <class T>
bool bsgm_multiscale_disparity_estimator::compute(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  const vil_image_view<bool>& invalid_tar,
  int min_disp,
  float invalid_disp,
  int const& multi_scale_mode,
  vil_image_view<float>& disp_tar,
  float dynamic_range_factor,
  bool skip_error_check)
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

  if(params_.use_shadow_step_p2_adjustment)
    if(shadow_step_prob_.ni()!=fine_w_ || shadow_step_prob_.nj()!=fine_h_){
      std::cout << "empty or wrong size shadow step" << std::endl;
      return false;
    }

  // Downsample the images
  vil_image_view<T> img_t_coarse, img_r_coarse,
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
     min_disp_img_coarse, invalid_disp_coarse, disp_t_coarse, dynamic_range_factor ) )
    return false;


  // Compute the median disparity for the entire target image
    int med_ds = bsgm_compute_median_of_image(
      disp_t_coarse, invalid_t_coarse, 0, coarse_w_, 0, coarse_h_,
      min_disp_coarse, num_coarse_disparities_, invalid_disp_coarse );

  // Mode 0 uses a constant min disparity (the median) for all pixels
  if( multi_scale_mode == 0 ){
    // Setup fine-scale min disparity image
    min_disp_img_fine.fill(
      med_ds*downscale_factor_ - num_active_disparities_/2 );

  // Mode 1 uses a fixed min disparity in large block regions
  } else if( multi_scale_mode == 1 ) {

    int coarse_block_size = block_size / downscale_factor_;

    for( int start_y = 0; start_y < coarse_h_; start_y+=coarse_block_size ){
      for( int start_x = 0; start_x < coarse_w_; start_x+=coarse_block_size ){

        int end_x = std::min( coarse_w_, start_x+coarse_block_size );
        int end_y = std::min( coarse_h_, start_y+coarse_block_size );

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
      disp_t_coarse = fill_1x1_holes(disp_t_coarse);
    // Set the min (fine-scale) disparity for each pixel to a value
    // value centered on this median.
    for( int y = 0; y < coarse_h_; y++ ){
      for( int x = 0; x < coarse_w_; x++ ){
        float d = disp_t_coarse(x,y);
        if(std::isnan(d)) d = med_ds;
        min_disp_img_coarse(x,y) =
          d*downscale_factor_ - num_active_disparities_/2;
      }
    }

    // Upscale to full-res
    vil_resample_bilin(
      min_disp_img_coarse, min_disp_img_fine, fine_w_, fine_h_ );
  }

  // Run fine-scale SGM
  if( !fine_de_->compute( img_tar, img_ref, invalid_tar,
      min_disp_img_fine, invalid_disp, disp_tar, dynamic_range_factor, skip_error_check ) )
    return false;
  
#if 0
  //DEBUG visualize problems with disparity search----
  // use coarse disparity as final result
  vil_image_view<float> temp_f_disp;
  vil_resample_bilin(disp_t_coarse, temp_f_disp, fine_w_, fine_h_);
  for (size_t j = 0; j < fine_h_; ++j)
      for (size_t i = 0; i < fine_w_; ++i)
          temp_f_disp(i, j) = downscale_factor_ * temp_f_disp(i, j);
  disp_tar = temp_f_disp; 
  std::cout << "MEDIAN " << med_ds << std::endl;
  vil_save(disp_tar, "c:/debug/final_disp_target.tif");
  //fine_de_->write_cost_debug_imgs( std::string("c:/debug/results"), true );
  //------
#endif
  return true;
}


//--------------------------------------------------------------
template <class T>
bool bsgm_multiscale_disparity_estimator::compute_both(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  const vil_image_view<bool>& invalid_tar,
  const vil_image_view<bool>& invalid_ref,
  int min_disparity,
  float invalid_disparity,
  int const& multi_scale_mode,
  vil_image_view<float>& disp_tar,
  vil_image_view<float>& disp_ref,
  float dynamic_range_factor)
{
  // Compute disparity maps for both images
  compute(img_tar, img_ref, invalid_tar, min_disparity,
  invalid_disparity, multi_scale_mode, disp_tar, dynamic_range_factor, true);
  compute(img_ref, img_tar, invalid_ref,
    (-num_fine_disparities_-min_disparity+1),
   invalid_disparity, multi_scale_mode, disp_ref, dynamic_range_factor,true);

  // Compute error maps
  vil_image_view<bool> error_tar, error_ref;
  bsgm_check_leftright(disp_tar, disp_ref, invalid_tar, invalid_ref, error_tar);
  bsgm_check_leftright(disp_ref, disp_tar, invalid_ref, invalid_tar, error_ref);

  // Set bad pixels to invalid
  for (size_t y = 0; y < img_tar.nj(); y++)
    for (size_t x = 0; x < img_tar.ni(); x++)
      if (error_tar(x, y)) disp_tar(x, y) = invalid_disparity;

  for (size_t y = 0; y < img_ref.nj(); y++)
    for (size_t x = 0; x < img_ref.ni(); x++)
      if (error_ref(x, y)) disp_ref(x, y) = invalid_disparity;

  // Interpolate
  if (params_.error_check_mode > 1) {
    bsgm_interpolate_errors(disp_tar, invalid_tar,
      img_tar, invalid_disparity, params_.shadow_thresh);
    bsgm_interpolate_errors(disp_ref, invalid_ref,
      img_ref, invalid_disparity, params_.shadow_thresh);
  }

  return true;
}


#endif // bsgm_multiscale_disparity_estimator_h_
