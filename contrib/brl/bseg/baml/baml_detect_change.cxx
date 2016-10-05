// This is brl/baml/baml_detect_change.cxx

#include <iomanip>
#include <algorithm>

#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>

#include <vil/algo/vil_greyscale_dilate.h>
#include <vil/algo/vil_greyscale_erode.h>
#include <vil/algo/vil_gauss_filter.h>

#include <vil/algo/vil_greyscale_erode.hxx>
#include <vil/algo/vil_greyscale_dilate.hxx>
VIL_GREYSCALE_ERODE_INSTANTIATE( vxl_uint_16 );
VIL_GREYSCALE_DILATE_INSTANTIATE( vxl_uint_16 );

#include "baml_detect_change.h"
#include "baml_census.h"



//-------------------------------------------------------------------
bool baml_detect_change_bt(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_prob,
  float bt_std,
  int bt_rad )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  float gauss_var = 1.0f/(bt_std*bt_std);
  float gauss_norm = 1.0f/(bt_std*sqrt(2*3.14159f)/255.0f);
   
  // Initialize output image
  tar_prob.set_size( width, height );
  tar_prob.fill( 0.0 );

  // Compute neighborhood min and max
  vil_image_view<vxl_uint_16> nbhd_min, nbhd_max;
  vil_structuring_element se; se.set_to_disk( bt_rad + 0.01 );
  vil_greyscale_dilate<vxl_uint_16>( img_ref, nbhd_max, se );
  vil_greyscale_erode<vxl_uint_16>( img_ref, nbhd_min, se );

  // Compute the Birchfield-Tomasi metric at each valid pixel
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( valid_ref(x,y) == false ) continue;

      float score = 0.0f;

      if( img_tar(x,y) < nbhd_min(x,y) ) 
        score = nbhd_min(x,y) - (float)img_tar(x,y);
      else if( img_tar(x,y) > nbhd_max(x,y) ) 
        score = img_tar(x,y) - (float)nbhd_max(x,y);

      tar_prob(x,y) = gauss_norm*exp( -score*score*gauss_var );

      //if( rand() < 0.001*RAND_MAX ) std::cerr << tar_prob(x,y) << ' ';
    }
  }

  return true;
}


//---------------------------------------------------------------
bool baml_detect_change_census(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_prob,
  float census_std,
  int census_tol,
  int census_rad )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  float gauss_var = 1.0f/(census_std*census_std);
  float gauss_norm = 1.0f/(census_std*sqrt(2*3.14159f));

  // Bound-check census_rad
  if( census_rad <= 0 ) census_rad = 1;
  if( census_rad > 3 ) census_rad = 3;
  int census_diam = census_rad*2+1;

  // Pre-build a census lookup table
  unsigned char lut[256];
  baml_generate_bit_set_lut( lut );
  bool only_32_bits = (census_diam <= 5);

  // Initialize output image
  tar_prob.set_size( width, height );
  tar_prob.fill( 0.0f );

  // Compute both census images
  vil_image_view<vxl_uint_64> census_tar, census_ref;
  vil_image_view<vxl_uint_64> salience_tar, salience_ref;

  // Convert 16-bit images to 8-bit
  // TEMPORARY until we have templated census function
  vil_image_view<vxl_byte> img_tar_8u, img_ref_8u;
  vil_convert_stretch_range_limited( img_tar, img_tar_8u, (vxl_uint_16)0, (vxl_uint_16)2000 );
  vil_convert_stretch_range_limited( img_ref, img_ref_8u, (vxl_uint_16)0, (vxl_uint_16)2000 );


  baml_compute_census_img( 
    img_tar_8u, census_diam, census_tar, salience_tar, census_tol );
  baml_compute_census_img( 
    img_ref_8u, census_diam, census_ref, salience_ref, census_tol );

  // Compute hamming distance between images
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( valid_ref(x,y) == false ) continue;
      
      unsigned long long int cen_diff = baml_compute_diff_string( 
        census_tar(x,y), census_ref(x,y), 
        salience_tar(x,y), salience_ref(x,y) );

      unsigned char ham = baml_compute_hamming_lut( cen_diff, lut, only_32_bits );
      tar_prob(x,y) = gauss_norm*exp( -ham*ham*gauss_var );

      //if( rand() < 0.001*RAND_MAX ) std::cerr << tar_prob(x,y) << ' ';
    }
  }

  return true;
}


//------------------------------------------------------------------------
bool baml_detect_change_mi(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_prob )
{
  // Hardcoded params
  float gauss_rad_percent = 0.03f;
  int img_bit_depth = 12;
  int hist_bit_depth = 10;

  int img_bit_ds = pow( 2, img_bit_depth-hist_bit_depth );

  int hist_range = pow( 2, hist_bit_depth );
  int gauss_rad = (int)( gauss_rad_percent*hist_range );
  double gauss_sd = gauss_rad/3.0;

  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  // Initialize output image
  tar_prob.set_size( width, height );
  tar_prob.fill( 0.0f );

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
    }
  }

  // Blur the transfer array to account for sampling issues
  vil_image_view<double> tf_blur;
  vil_gauss_filter_2d( tf_raw, tf_blur, gauss_sd, gauss_rad );

  vil_image_view<float> tf( hist_range, hist_range ); 
  tf.fill( 0.0f );

  // Normalize probabilities to compute a final transfer function
  for( int r = 1; r < hist_range; r ++ ){  
    
    double tar_max = 0.0;
    for( int t = 1; t < hist_range; t++ )
      tar_max = std::max( tar_max, tf_blur(t,r) );
    
    for( int t = 1; t < hist_range; t++ )
      tf(t,r) = tf_blur(t,r)/tar_max;
  }

  // One more pass to look up appearance prob
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( !valid_ref(x,y) ) continue;
      int tx = (int)( img_tar(x,y)/img_bit_ds );
      int ty = (int)( img_ref(x,y)/img_bit_ds );
      tar_prob(x,y) = tf( tx, ty );
    } //x
  } //y

std::cerr << "baml_detect_change_mi HACKED!\n";
vil_image_view<vxl_byte> vis;
vil_convert_stretch_range_limited( tf, vis, 0.0f, 1.0f );
vil_save( vis, "D:/results/b.png" );

  return true;
}


//------------------------------------------------------------------
bool baml_overlay_red(
  const vil_image_view<vxl_byte>& img,
  const vil_image_view<vxl_byte>& map,
  vil_image_view<vxl_byte>& vis_img,
  vxl_byte clear_map_val,
  vxl_byte red_map_val )
{
  if( img.ni() != map.ni() || img.nj() != map.nj() )
    return false;

  vis_img.set_size( map.ni(), map.nj(), 3 );

  float map_norm = 1.0f/(red_map_val-(float)clear_map_val);
  
  for( unsigned y = 0; y < map.nj(); y++ ){
    for( unsigned x = 0; x < map.ni(); x++ ){
      float relativeVal = std::min( 1.0f, std::max( 0.0f, 
        map_norm*((float)map(x,y)) - (float)clear_map_val ));

      vis_img(x,y,0) = vis_img(x,y,1) = (vxl_byte)( 
        img(x,y)*(1.0-relativeVal) );
      vis_img(x,y,2) = (vxl_byte)( 
        img(x,y)*(1.0-relativeVal) + 255*relativeVal );
    }
  }

  return true;
}

