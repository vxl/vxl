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
  vil_image_view<float> const& shadow_step_prob,
  vil_image_view<float> const& shadow_prob,
  vgl_vector_2d<float> sun_dir_tar,
  int downscale_exp ) :
    fine_w_( img_width ),
    fine_h_( img_height ),
    num_fine_disparities_( num_disparities ),
    num_active_disparities_( num_active_disparities ),
    shadow_step_prob_(shadow_step_prob),
    shadow_prob_(shadow_prob),
    sun_dir_tar_(sun_dir_tar),
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
    // Downsample the images
  if(params_.use_shadow_step_p2_adjustment){
    // target shadow step
    vil_image_view<float> temp_ss_coarse, temp_sh_coarse, working;
    vil_gauss_reduce( shadow_step_prob_, ss_coarse_, working );
    for( int s = 1; s < downscale_exponent_; s++ ){
      vil_gauss_reduce( ss_coarse_, temp_ss_coarse, working );
      ss_coarse_.deep_copy( temp_ss_coarse );
    }
    // shadow 
    vil_gauss_reduce(shadow_prob_, sh_coarse_, working);
    for (int s = 1; s < downscale_exponent_; s++) {
        vil_gauss_reduce(sh_coarse_, temp_sh_coarse, working);
        sh_coarse_.deep_copy(temp_sh_coarse);
    }
  }
  coarse_de_ = new bsgm_disparity_estimator(params, coarse_w_, coarse_h_, num_coarse_disparities_ , ss_coarse_, sh_coarse_, sun_dir_tar_);
  fine_de_ = new bsgm_disparity_estimator(
    params, fine_w_, fine_h_, num_active_disparities, shadow_step_prob_, shadow_prob_, sun_dir_tar_);
}


//----------------------------------------------------------------------------
bsgm_multiscale_disparity_estimator::~bsgm_multiscale_disparity_estimator()
{
  delete coarse_de_;
  delete fine_de_;
}

vil_image_view<float> bsgm_multiscale_disparity_estimator::fill_1x1_holes(vil_image_view<float> const& img) {
    float large_spike_tol = 10.0f;
    size_t ni = img.ni(), nj = img.nj();
    vil_image_view<float> ret(img);
    // ignore borders
    for(int j = 1; j<(nj-1); ++j)
        for (int i = 1; i < (ni - 1); ++i) {
            float sum = 0.0f, ns = 0.0f;
            for (int rj = -1; rj <= 1; ++rj)
                for (int ri = -1; ri <= 1; ++ri)
            {
                if (ri == 0&&rj ==0) continue;
                float v = img(i + ri, j + rj);
                if (std::isnan(v))
                    continue; 
                sum += v; ns += 1.0f;
            }
            if (ns < 8.0) {
                continue;
            }
            sum /= ns;
            float v = img(i, j);
            if (std::isnan(v)) {
                ret(i, j) = sum;
                continue;
            }
            if (fabs(v - sum) > large_spike_tol)
                ret(i, j) = sum;
       }
    return ret;
}
//----------------------------------------------------------------------------
