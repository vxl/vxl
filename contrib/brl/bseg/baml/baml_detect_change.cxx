// This is brl/baml/baml_detect_change.cxx

#include <iomanip>
#include <algorithm>

#include <vnl/vnl_inverse.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_gauss_filter.h>

#include "baml_birchfield_tomasi.h"
#include "baml_detect_change.h"
#include "baml_census.h"
#include "baml_utilities.h"


//-------------------------------------------------------------------
bool baml_change_detection::detect(
  const vil_image_view<vxl_uint_16>& img_target,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid,
  vil_image_view<float>& change_prob_target )
{
  // Correct gain/offset
  vil_image_view<vxl_uint_16> corr_ref;
  if( params_.correct_gain_offset )
    baml_correct_gain_offset_tiled( img_target, img_ref, valid, params_.num_tiles, corr_ref );
  else
    corr_ref.deep_copy( img_ref );

  bool dc_success = false;

  // Find appropriate translational offsets
  vil_image_view<vxl_uint_16> img_tar_crop;
  vil_image_view<vxl_uint_16> img_ref_crop;
  vil_image_view <bool> valid_crop;
  vil_image_view<float> lh;
  vil_image_view<float> lh_crop;
  vil_image_view<float> lh_best;
  lh.set_size(img_ref.ni(), img_ref.nj());
  lh_best.set_size(img_ref.ni(), img_ref.nj());
  int tar_x_off;
  int tar_y_off;
  int ref_x_off;
  int ref_y_off;
  int crop_n_i;
  int crop_n_j;
  float min_mean = pow(2, 16) - 1;
  float mean_score = 0;
  
  // try all offsets within the selected translational radius
  for (int x_off = -params_.registration_refinement_rad; x_off <= params_.registration_refinement_rad; x_off++) {
    for (int y_off = -params_.registration_refinement_rad; y_off <= params_.registration_refinement_rad; y_off++) {
      // determine cropping offsets and size
      if (x_off < 0) {
        ref_x_off = -x_off;
        tar_x_off = 0;
        crop_n_i = img_ref.ni() + x_off;
      }
      else {
        ref_x_off = 0;
        tar_x_off = x_off;
        crop_n_i = img_ref.ni() - x_off;
      }
      if (y_off < 0) {
        ref_y_off = -y_off;
        tar_y_off = 0;
        crop_n_j = img_ref.nj() + y_off;
      }
      else {
        ref_y_off = 0;
        tar_y_off = y_off;
        crop_n_j = img_ref.nj() - y_off;
      }
      lh.fill(0.0);
      img_tar_crop = vil_crop(img_target, tar_x_off, crop_n_i, tar_y_off, crop_n_j);
      img_ref_crop = vil_crop(corr_ref, ref_x_off, crop_n_i, ref_y_off, crop_n_j);
      valid_crop = vil_crop(valid, tar_x_off, crop_n_i, tar_y_off, crop_n_j);
      lh_crop = vil_crop(lh, tar_x_off, crop_n_i, tar_y_off, crop_n_j);

      // Detect change using specified method
      if (params_.method == BIRCHFIELD_TOMASI)
        dc_success = detect_bt(
          img_tar_crop, img_ref_crop, valid_crop, lh_crop);
      else if (params_.method == CENSUS)
        dc_success = detect_census(
          img_tar_crop, img_ref_crop, valid_crop, lh_crop);
      else if( params_.method == DIFFERENCE) 
        dc_success = detect_difference(
          img_tar_crop, img_ref_crop, valid_crop, lh_crop);
      else if (params_.method == GRADIENT_DIFF)
        dc_success = detect_gradient(
          img_tar_crop, img_ref_crop, valid_crop, lh_crop);
      else if (params_.method == NON_PARAMETRIC)
        dc_success = detect_nonparam(
          img_tar_crop, img_ref_crop, valid_crop, lh_crop);
      // save the score image if it improved the results 
      vil_math_mean(mean_score, lh_crop, 0);
      if (mean_score<min_mean) {
        change_prob_target.deep_copy(lh_crop);
        min_mean = mean_score;
      }
    }

  }
  if( !dc_success ) return false;
  /*for (int x = 0; x < change_prob_target.ni(); x++) {
    for (int y = 0; y < change_prob_target.nj(); y++) {
      std::cout << change_prob_target(x, y) << "\n";
    }
  }*/
  // Convert likelihood into probability
  baml_sigmoid( 
    change_prob_target, change_prob_target, params_.prior_change_prob );  return true;
}

//-------------------------------------------------------------------
bool 
baml_change_detection::detect_bt(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  float gauss_var = params_.bt_std*params_.bt_std;
  float gauss_norm = log( 1.0f/(params_.bt_std*sqrt(2*3.14159f)/255.0f) );
   
  // Initialize output image
  tar_lh.set_size( width, height );
  tar_lh.fill( 0.0 );

  // Compute min and max observed intensities in target image
  vxl_uint_16 min_int = (vxl_uint_16)(pow(2,16)-1);
  vxl_uint_16 max_int = (vxl_uint_16)0;
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( valid_ref(x,y) == false ) continue;
      min_int = std::min( min_int, img_tar(x,y) );
      max_int = std::max( max_int, img_tar(x,y) );
    }
  }

  // Compute foreground likelihood assuming uniform distribution on foreground
  float lfg = log( 1.0f/(max_int-min_int) );

  // Compute Birchfield-Tomasi score
  vil_image_view<vxl_uint_16> score;
  if( !baml_compute_birchfield_tomasi( 
    img_tar, img_ref, score, params_.bt_rad ) )
    return false;

  // Convert BT score to log likelihood ratio
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( valid_ref(x,y) == false ) continue;
      float lbg = gauss_norm - score(x,y)*(float)score(x,y)/gauss_var;
      tar_lh(x,y) = lfg - lbg;
    }
  }

  return true;
}


//---------------------------------------------------------------
bool 
baml_change_detection::detect_census(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;
  // Bound-check census_rad
  if( params_.census_rad <= 0 ) params_.census_rad = 1;
  if( params_.census_rad > 3 ) params_.census_rad = 3;
  int census_diam = params_.census_rad*2+1;

  // Get parameters for background Gaussian distribution
  float gauss_std = params_.census_std*census_diam*census_diam;
  float gauss_var = gauss_std*gauss_std;
  float gauss_norm = log( 1.0f/(gauss_std*sqrt(2*3.14159f)) );

  // Pre-build a census lookup table
  unsigned char lut[256];
  baml_generate_bit_set_lut( lut );
  bool only_32_bits = (census_diam <= 5);

  // Initialize output image
  tar_lh.set_size( width, height );
  tar_lh.fill( 1.0f );

  // Compute foreground likelihood assuming uniform distribution on foreground
  float lfg = log( 1.0f/(census_diam*census_diam) );

  // Compute both census images
  vil_image_view<vxl_uint_64> census_tar, census_ref;
  vil_image_view<vxl_uint_64> salience_tar, salience_ref;

  baml_compute_census_img( 
    img_tar, census_diam, census_tar, salience_tar, params_.census_tol );
  baml_compute_census_img( 
    img_ref, census_diam, census_ref, salience_ref, params_.census_tol );

  // Compute hamming distance between images
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( valid_ref(x,y) == false ) continue;
      
      unsigned long long int cen_diff = baml_compute_diff_string( 
        census_tar(x,y), census_ref(x,y), 
        salience_tar(x,y), salience_ref(x,y) );

      unsigned char ham = baml_compute_hamming_lut( cen_diff, lut, only_32_bits );
      float lbg = gauss_norm - ham*(float)ham/gauss_var;

      tar_lh(x,y) = lfg - lbg;
      
      //if( rand() < 0.001*RAND_MAX ) std::cerr << (int)ham << ' ';
    }
  }
  return true;
}

//---------------------------------------------------------------
bool baml_change_detection::detect_difference(const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh)
{
  int width = img_tar.ni(), height = img_tar.nj();

  if (img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height)
    return false;

  // Initialize output image
  tar_lh.set_size(width, height);
  tar_lh.fill(1.0f);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (img_ref(x, y) > img_tar(x, y))
        tar_lh(x, y) = img_ref(x, y) - img_tar(x, y);
      else
        tar_lh(x, y) = img_tar(x, y) - img_ref(x, y);
    }
  }
  return true;
}

//---------------------------------------------------------------
bool 
baml_change_detection::detect_gradient(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh )
{
  float mag_tol = 0.00001f;
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  // Get parameters for background Gaussian distribution
  float gauss_var = params_.grad_std*params_.grad_std;
  float gauss_norm = log( 1.0f/(params_.grad_std*sqrt(2*3.14159f)) );

  // Initialize output image
  tar_lh.set_size( width, height );
  tar_lh.fill( 1.0f );

  // Compute foreground likelihood assuming uniform distribution on foreground
  float lfg = log( 1.0f/(4*params_.grad_std) );

  // Compute gradient images
  vil_image_view<float> grad_x_tar, grad_y_tar, grad_x_ref, grad_y_ref;
  vil_sobel_3x3<vxl_uint_16,float>( img_tar, grad_x_tar, grad_y_tar );
  vil_sobel_3x3<vxl_uint_16,float>( img_ref, grad_x_ref, grad_y_ref );

  // Compute hamming distance between images
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( valid_ref(x,y) == false ) continue;
      
      float grad_mag_tar = sqrt(
        grad_x_tar(x,y)*grad_x_tar(x,y) + grad_y_tar(x,y)*grad_y_tar(x,y) );
      float grad_mag_ref = sqrt( 
        grad_x_ref(x,y)*grad_x_ref(x,y) + grad_y_ref(x,y)*grad_y_ref(x,y) );
      float grad_ip = grad_x_tar(x,y)*grad_x_ref(x,y) + grad_y_tar(x,y)*grad_y_ref(x,y);
      // if the magnitudes are large enough compute their angle 
      // difference, otherwise leave at default 90 degrees
      float angle_diff = 3.14159/2.0;
      if( grad_mag_tar > mag_tol && grad_mag_ref > mag_tol )
        angle_diff = acos( grad_ip/(grad_mag_tar*grad_mag_ref) );

      //float grad_diff = pow( grad_mag_tar - grad_mag_ref, 2 );

      //float grad_diff = pow( grad_x_tar(x,y)-grad_x_ref(x,y), 2 ) + pow( grad_y_tar(x,y)-grad_y_ref(x,y), 2 );

      float grad_diff = grad_mag_tar*grad_mag_ref - grad_ip;

      //float grad_diff = pow( std::max( grad_mag_tar, grad_mag_ref )*sin(angle_diff), 2 );
      //float grad_diff = pow( 0.5f*( grad_mag_tar+grad_mag_ref )*fabs( sin(angle_diff) ), 2 );

      float lbg = gauss_norm - grad_diff/gauss_var;

      tar_lh(x, y) = lfg - lbg;
      if (rand() < 0.001*RAND_MAX) std::cerr << tar_lh(x, y) << "\n";
      if( rand() < 0.001*RAND_MAX ) std::cerr << (int)angle_diff << "\n";
    }
  }

  return true;
}

//------------------------------------------------------------------------
bool 
baml_change_detection::detect_nonparam(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh )
{
  // Hardcoded params
  float gauss_rad_percent = 0.01f;
  double double_tol = 0.000000001;

  int img_bit_ds = pow( 2, params_.img_bit_depth-params_.hist_bit_depth );

  int hist_range = pow( 2, params_.hist_bit_depth );
  int gauss_rad = (int)( gauss_rad_percent*hist_range );
  double gauss_sd = gauss_rad/3.0;

  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  // Initialize output image
  tar_lh.set_size( width, height );
  tar_lh.fill( 0.0f );

  int min_bin = hist_range, max_bin = 0;

  // Setup a transfer function array
  vil_image_view<double> tf_raw( (int)hist_range, (int)hist_range );
  tf_raw.fill( 0.0 );

  // Populate the transfer function
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( !valid_ref(x,y) ) continue;

      int tx = (int)( img_tar(x,y)/img_bit_ds );
      int ty = (int)( img_ref(x,y)/img_bit_ds );
      if( tx >= hist_range || ty >= hist_range ){
        std::cerr << "ERROR: baml_detect_change_mi, observed intensity "
          << img_tar(x,y) << ' ' << img_ref(x,y) << " larger than expected bit range, aborting\n";
        return false;
      }
      tf_raw( tx, ty ) += 1.0;

      // Record min and max bin
      min_bin = std::min( min_bin, tx );
      max_bin = std::max( max_bin, tx );
    }
  }

  // Pre-compute probability of foreground
  double lfg = log( 1.0/(max_bin-min_bin) );

  // Blur the transfer array to account for sampling issues
  vil_image_view<double> tf_blur;
  vil_gauss_filter_2d( tf_raw, tf_blur, gauss_sd, gauss_rad );

  vil_image_view<float> lbg( hist_range, hist_range ); 
  lbg.fill( 0.0f );

  // Normalize transfer function to compute probability of background
  for( int r = 1; r < hist_range; r ++ ){  
    
    double tar_sum = 0.0;
    for( int t = 1; t < hist_range; t++ )
      tar_sum = tar_sum + tf_blur(t,r);
    
    if( tar_sum < double_tol ) continue;

    for( int t = 1; t < hist_range; t++ ){
      if( tf_blur(t,r) < double_tol ) lbg(t,r) = log( double_tol );
      else lbg(t,r) = log( tf_blur(t,r)/tar_sum );
    }
  }

  // One more pass to look up appearance prob
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( !valid_ref(x,y) ) continue;
      int tx = (int)( img_tar(x,y)/img_bit_ds );
      int ty = (int)( img_ref(x,y)/img_bit_ds );
      tar_lh(x,y) = lfg - lbg( tx, ty );
    } //x
  } //y

/*std::cerr << "baml_detect_change_nonparam HACKED!\n";
vil_image_view<vxl_byte> vis;
vil_convert_stretch_range_limited( lbg, vis, -10.0f, 0.0f );
vil_save( vis, "D:/results/b.png" );*/

  return true;
}

