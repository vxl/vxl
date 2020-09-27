// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

#include <iomanip>
#include <algorithm>
#include <cmath>

#include "vil/vil_save.h"
#include "vil/vil_convert.h"
#include "bsgm_multiscale_disparity_estimator.h"
#include "bsgm_error_checking.h"

using namespace std;


//----------------------------------------------------------------------------
int bsgm_multiscale_disparity_estimator::bsgm_compute_median_of_image(
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
      if (isnan(invalid_disparity)) {
        if (isnan(img(x, y)))
          continue;
      }
      else if( (img(x,y) == invalid_disparity) || invalid(x,y) ) continue;

      int sample = static_cast<int>(( img(x,y) - min_img_val ));
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
    num_active_disparities_( num_active_disparities ),
    params_(params)
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
