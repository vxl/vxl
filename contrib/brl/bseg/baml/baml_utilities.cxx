// This is brl/baml/baml_utilities.cxx

#include <iomanip>
#include <algorithm>

#include <vnl/vnl_inverse.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_gauss_filter.h>

#include "baml_utilities.h"


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

