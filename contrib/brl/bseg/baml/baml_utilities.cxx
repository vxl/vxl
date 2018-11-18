// This is brl/baml/baml_utilities.cxx

#include <iomanip>
#include <algorithm>

#include <vnl/vnl_inverse.h>
#include <vnl/vnl_math.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_crop.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/vil_resample_bicub.h>
#include <vil/vil_resample_bilin.h>

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
bool baml_correct_gain_offset_tiled(
  const vil_image_view<vxl_uint_16>& img_tar,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid_ref,
  int num_tiles,
  vil_image_view<vxl_uint_16>& corrected_ref)
{
  // calculate gain/offset over whole image if the number of tiles is <= 1
  if (num_tiles <= 1)
    return baml_correct_gain_offset(img_tar, img_ref, valid_ref, corrected_ref);

  // initialization
  int width = img_tar.ni(), height = img_tar.nj();
  vil_image_view<float> gain;
  vil_image_view<float> offset;
  vil_image_view<float> gain_resized;
  vil_image_view<float> offset_resized;
  vnl_vector<int> boundaries_x;
  vnl_vector<int> boundaries_y;
  gain.set_size(num_tiles, num_tiles);
  offset.set_size(num_tiles, num_tiles);
  boundaries_x.set_size(num_tiles+1);
  boundaries_y.set_size(num_tiles+1);
  corrected_ref.set_size(width, height);
  corrected_ref.fill(0);
  gain_resized.set_size(width, height);
  gain_resized.fill(0);
  offset_resized.set_size(width, height);
  offset_resized.fill(0);

  // calculate tile boundaries
  boundaries_x(0) = 0;
  boundaries_y(0) = 0;
  for (int i = 1; i <= num_tiles; i++) {
    boundaries_x(i) = static_cast<int>(width*i / num_tiles+0.5);
    boundaries_y(i) = static_cast<int>(height*i / num_tiles+0.5);
  }

  vil_image_view<vxl_uint_16> tar_crop;
  vil_image_view<vxl_uint_16> ref_crop;
  vil_image_view<bool> valid_ref_crop;

  // loop over each tile, computing a gain/offset
  for (int tile_row = 0; tile_row < num_tiles; tile_row++) {
    for (int tile_col = 0; tile_col < num_tiles; tile_col++) {

      tar_crop = vil_crop(img_tar, boundaries_x(tile_col), boundaries_x(tile_col + 1) - boundaries_x(tile_col),
        boundaries_y(tile_row), boundaries_y(tile_row + 1) - boundaries_y(tile_row));
      ref_crop = vil_crop(img_ref, boundaries_x(tile_col), boundaries_x(tile_col + 1) - boundaries_x(tile_col),
        boundaries_y(tile_row), boundaries_y(tile_row + 1) - boundaries_y(tile_row));
      valid_ref_crop = vil_crop(valid_ref, boundaries_x(tile_col), boundaries_x(tile_col + 1) - boundaries_x(tile_col),
        boundaries_y(tile_row), boundaries_y(tile_row + 1) - boundaries_y(tile_row));

      // Compute statistics over the image
      double sumw = 0, sumI1 = 0, sumI2 = 0;
      double sumI1I2 = 0, sumI2Sq = 0;

      for (int y = 0; y < tar_crop.nj(); y++) {
        for (int x = 0; x < tar_crop.ni(); x++) {
          if (!valid_ref_crop(x, y)) continue;
          float i1 = tar_crop(x, y) / 255.0f;
          float i2 = ref_crop(x, y) / 255.0f;

          sumw += 1.0;
          sumI1 += i1;
          sumI2 += i2;
          sumI1I2 += i2*i1;
          sumI2Sq += i2*i2;
        }
      }

      // Check good stats
      if (sumw < 1.0) return false;

      // Compute weighted least squares estimate of gain/offset
      vnl_matrix_fixed<double, 2, 2> A;
      A(0, 0) = sumI2Sq / sumw; A(0, 1) = sumI2 / sumw;
      A(1, 0) = sumI2 / sumw; A(1, 1) = 1.0;

      vnl_matrix_fixed<double, 2, 1> b;
      b(0, 0) = sumI1I2 / sumw; b(1, 0) = sumI1 / sumw;

      // save the gain and offset to be interpolated later
      vnl_matrix_fixed<double, 2, 1> c = vnl_inverse(A)*b;
      gain(tile_row, tile_col) = c(0, 0);
      offset(tile_row, tile_col) = c(1, 0) * 255;
    }
  }

  // create (replicate) padded versions of the gain and offset ( for input to bicubic interpolation function )
  int pad_size = 2;
  vil_image_view<float> gain_pad;
  gain_pad.set_size(gain.ni() + pad_size*2, gain.nj() + pad_size*2);
  vil_image_view<float> offset_pad;
  offset_pad.set_size(offset.ni() + pad_size*2, offset.nj() + pad_size*2);
  for (int x = pad_size; x < gain_pad.ni() - pad_size; x++) {
    for (int p = 0; p < pad_size; p++) {
      gain_pad(p, x) = gain(0, x - pad_size);
      gain_pad(gain_pad.ni() - 1 - p, x) = gain(gain.ni() - 1, x - pad_size);
      gain_pad(x, p) = gain(x - pad_size, 0);
      gain_pad(x, gain_pad.nj() - 1 - p) = gain(x - pad_size, gain.nj() - 1);

      offset_pad(p, x) = offset(0, x - pad_size);
      offset_pad(offset_pad.ni() - 1 - p, x) = offset(offset.ni() - 1, x - pad_size);
      offset_pad(x, p) = offset(x - pad_size, 0);
      offset_pad(x, offset_pad.nj() - 1 - p) = offset(x - pad_size, offset.nj() - 1);
    }
    for (int y = pad_size; y < gain_pad.nj() - pad_size; y++) {
      gain_pad(x, y) = gain(x - pad_size, y - pad_size);
      offset_pad(x, y) = offset(x - pad_size, y - pad_size);

    }
  }
  for (int p1 = 0; p1 < pad_size; p1++) {
    for (int p2 = 0; p2 < pad_size; p2++) {
      gain_pad(p1, p2) = gain(0, 0);
      gain_pad(p1, gain_pad.nj() - 1 - p2) = gain(0, gain.nj() - 1);
      gain_pad(gain_pad.ni() - 1 - p1, p2) = gain(gain.ni() - 1, 0);
      gain_pad(gain_pad.ni() - 1 - p1, gain_pad.nj() - 1 - p2) = gain(gain.ni() - 1, gain.nj() - 1);
      offset_pad(p1, p2) = offset(0, 0);
      offset_pad(p1, offset_pad.nj() - 1 - p2) = offset(0, offset.nj() - 1);
      offset_pad(offset_pad.ni() - 1 - p1, p2) = offset(offset.ni() - 1, 0);
      offset_pad(offset_pad.ni() - 1 - p1, offset_pad.nj() - 1 - p2) = offset(offset.ni() - 1, offset.nj() - 1);
    }
  }

  // use padded versions to resize the images
  double f = 1.0;
  double x0 = double(pad_size)-0.5;
  double y0 = double(pad_size)-0.5;
  double dx1 = f*(gain.ni())*1.0 / (width - 1);
  double dy1 = 0;
  double dx2 = 0;
  double dy2 = f*(gain.nj())*1.0 / (height - 1);
  vil_resample_bilin(gain_pad, gain_resized, x0, y0, dx1, dy1, dx2, dy2, width, height);
  vil_resample_bilin(offset_pad, offset_resized, x0, y0, dx1, dy1, dx2, dy2, width, height);

  // Apply the gain offset
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (!valid_ref(x, y)) continue;
      float r = std::max(0.0f, img_ref(x, y)*gain_resized(x, y) + offset_resized(x, y));
      corrected_ref(x, y) = (vxl_uint_16) r;
    }
  }
  return true;
}

//----------------------------------------------------------
// convert scores to probability using sigmoid
void baml_sigmoid(
  const vil_image_view<float>& lh,
  vil_image_view<float>& prob,
  float prior_prob )
{
  int width = lh.ni(), height = lh.nj();

  float log_prior = log(prior_prob / (1.0f - prior_prob));

  // Initialize output image
  prob.set_size( width, height );

  for( int y = 0; y < height; y++ ){
    for( int x = 0; x < width; x++ ){
      //prob(x,y) = prior_prob/( prior_prob + (1.0f-prior_prob)*exp( -lh(x,y) ) );
      prob(x, y) = 1.0f / (1.0f + exp(-(log_prior+lh(x, y))));
    }
  }
}

//------------------------------------------------------------------
// convert scores to probability with gaussian
void baml_gaussian(
  const vil_image_view<float>& scores,
  vil_image_view<float>& prob,
  float sigma )
{
  int width = scores.ni();
  int height = scores.nj();
  // Initialize output image
  prob.set_size(width, height);
  prob.fill(0.0);
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      prob(x, y) = 1 / (sqrt(2 * 3.14159265358979323846)*sigma)*exp(-pow(scores(x, y), 2) / (2 * pow(sigma, 2)));
    }
  }
}

//------------------------------------------------------------------
// estimate sigma for use in gaussian assuming data is zero mean
float baml_sigma(
  const vil_image_view<float>& scores )
{
  int width = scores.ni(); int height = scores.nj();
  float sigma_sum = 0;
  float max_score = FLT_MIN;
  float min_score = FLT_MAX;
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        if (max_score < scores(x, y)) max_score = scores(x, y);
        if (min_score > scores(x, y)) min_score = scores(x, y);
        if (vnl_math::isnan(scores(x, y))) {
          std::cerr << "score is nan";
          return false;
        }
        if (vnl_math::isinf(scores(x, y))) {
          std::cerr << "score is infinity";
          return false;
        }
        sigma_sum += pow(scores(x, y), 2);
      }
    }
    float sigma = sqrt(sigma_sum / (width*height - 1));
    return sigma;
}

//------------------------------------------------------------------
// img: original image ( this will be the backgroud )
// map: score image ( this will be the red portion )
// vis_img: where the overlayed image will be stored
// clear_map_val: scores below this value will be completely clear
// red_map_val: scores above this value will be completely red
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

      vis_img(x,y,1) = vis_img(x,y,2) = (vxl_byte)(
        img(x,y)*(1.0-relativeVal) );
      vis_img(x,y,0) = (vxl_byte)(
        img(x,y)*(1.0-relativeVal) + 255*relativeVal );
    }
  }

  return true;
}
