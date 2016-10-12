// This is brl/baml/baml_detect_change.cxx

#include <iomanip>
#include <algorithm>

#include <vnl/vnl_inverse.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_gauss_filter.h>

#include "baml_birchfield_tomasi.h"
#include "baml_detect_change.h"
#include "baml_census.h"



//----------------------------------------------------------
bool baml_correct_gain_offset(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<vxl_uint_16>& corrected_ref )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  // Initialize output image
  corrected_ref.set_size( width, height );
  corrected_ref.fill( 0.0 );

  // Compute statistics over the image
  double sumw = 0, sumI1 = 0, sumI2 = 0;
  double sumI1I2 = 0, sumI2Sq = 0;

  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( !valid_ref(x,y) ) continue;

      float i1 = img_tar(x,y)/255.0f;
      float i2 = img_ref(x,y)/255.0f;

      sumw += 1.0;
      sumI1 += i1;
      sumI2 += i2;
      sumI1I2 += i2*i1;
      sumI2Sq += i2*i2;
    }
  }

  // Check good stats
  if( sumw < 1.0 ) return false;

  // Compute weighted least squares estimate of gain/offset
  vnl_matrix_fixed<double,2,2> A;
  A(0,0) = sumI2Sq/sumw; A(0,1) = sumI2/sumw;
  A(1,0) = sumI2/sumw; A(1,1) = 1.0;

  vnl_matrix_fixed<double,2,1> b;
  b(0,0) = sumI1I2/sumw; b(1,0) = sumI1/sumw;

  vnl_matrix_fixed<double,2,1> c = vnl_inverse( A )*b;
  double gain = c(0,0), offset = c(1,0)*255;

  std::cerr << "Gain: " << gain << "  Offset: " << offset << '\n';

  // Apply the gain offset
  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      if( !valid_ref(x,y) ) continue;
       
      double r = std::max( 0.0, img_ref(x,y)*gain + offset );
      corrected_ref(x,y) = (vxl_uint_16)r;
    }
  }

  return true;
}


//-------------------------------------------------------------------
bool baml_detect_change_bt(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh,
  float bt_std,
  int bt_rad )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;

  float gauss_var = bt_std*bt_std;
  float gauss_norm = log( 1.0f/(bt_std*sqrt(2*3.14159f)/255.0f) );
   
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
  if( !baml_compute_birchfield_tomasi( img_tar, img_ref, score, bt_rad ) )
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
bool baml_detect_change_census(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh,
  float census_std,
  int census_tol,
  int census_rad )
{
  int width = img_tar.ni(), height = img_tar.nj();

  if( img_ref.ni() != width || img_ref.nj() != height ||
    valid_ref.ni() != width || valid_ref.nj() != height )
    return false;
  
  // Bound-check census_rad
  if( census_rad <= 0 ) census_rad = 1;
  if( census_rad > 3 ) census_rad = 3;
  int census_diam = census_rad*2+1;

  // Get parameters for background Gaussian distribution
  float gauss_std = census_std*census_diam*census_diam;
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

  // Convert 16-bit images to 8-bit
  // TEMPORARY until we have templated census function
  //vil_image_view<vxl_byte> img_tar_8u, img_ref_8u;
  //vil_convert_stretch_range_limited( img_tar, img_tar_8u, (vxl_uint_16)0, (vxl_uint_16)2000 );
  //vil_convert_stretch_range_limited( img_ref, img_ref_8u, (vxl_uint_16)0, (vxl_uint_16)2000 );

  baml_compute_census_img( 
    img_tar, census_diam, census_tar, salience_tar, census_tol );
  baml_compute_census_img( 
    img_ref, census_diam, census_ref, salience_ref, census_tol );

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


//------------------------------------------------------------------------
bool baml_detect_change_nonparam(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  vil_image_view<float>& tar_lh,
  int img_bit_depth,
  int hist_bit_depth )
{
  // Hardcoded params
  float gauss_rad_percent = 0.01f;
  double double_tol = 0.000000001;

  int img_bit_ds = pow( 2, img_bit_depth-hist_bit_depth );

  int hist_range = pow( 2, hist_bit_depth );
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


//----------------------------------------------------------
void baml_sigmoid(
  const vil_image_view<float>& lh,
  vil_image_view<float>& prob,
  float prior_prob )
{
  int width = lh.ni(), height = lh.nj();

  // Initialize output image
  prob.set_size( width, height );

  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      prob(x,y) = prior_prob/( prior_prob + (1.0f-prior_prob)*exp( -lh(x,y) ) );
    }
  }
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

