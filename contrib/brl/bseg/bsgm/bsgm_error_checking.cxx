// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

//#include <iomanip>
#include <algorithm>
#include <vector>
#include <cmath>

//#include <vil/vil_save.h>
//#include <vil/vil_convert.h>
//#include <vil/algo/vil_structuring_element.h>
//#include <vil/algo/vil_sobel_3x3.h>
//#include <vil/algo/vil_median.h>
//#include <vil/algo/vil_binary_erode.h>
//#include <vil/algo/vil_gauss_reduce.h>

#include "bsgm_error_checking.h"

//-----------------------------------------------------------------------------
void
bsgm_check_nonunique(
  vil_image_view<float>& disp_img,
  const vil_image_view<unsigned short>& disp_cost,
  const vil_image_view<vxl_byte>& img,
  float invalid_disparity,
  vxl_byte shadow_thresh,
  int disp_thresh)
{
  int w = disp_img.ni(), h = disp_img.nj();
  std::vector<unsigned short> inv_cost(w);
  std::vector<int> inv_disp(w);

  for( int y = 0; y < h; y++ ){

    // Initialize an inverse disparity map for this row
    for( int x = 0; x < w; x++ ){
      inv_cost[x] = 65535;
      inv_disp[x] = -1;
    }

    // Construct the inverse disparity map
    for( int x = 0; x < w; x++ ){

      if (std::isnan(invalid_disparity)) {
        if (std::isnan(disp_img(x, y)))
          continue;
      }
      else if( disp_img(x,y) == invalid_disparity ) continue;

      // Get an integer disparity and location in reference image
      int d = static_cast<int>(std::round( disp_img(x, y) ));
      int x_l = x + d;

      if( x_l < 0 || x_l >= w ) continue;

      // Record the min cost pixel mapping back to x_l
      if( inv_cost[x_l] > disp_cost(x,y) ){
        inv_cost[x_l] = disp_cost(x,y);
        inv_disp[x_l] = d;
      }
    } //x

    // Check the uniqueness of each disparity.
    for( int x = 0; x < w; x++ ){

      if (std::isnan(invalid_disparity)) {
        if (std::isnan(disp_img(x, y)))
          continue;
      }
      else if( disp_img(x,y) == invalid_disparity ) continue;

      // Label dark pixels as invalid, if thresh=0 nothing happens
      if (img(x, y) < shadow_thresh) {
        disp_img(x, y) = invalid_disparity;
        continue;
      }

      // Compute the floor and ceiling of each disparity
      int d_floor = static_cast<int>(std::floor(disp_img(x,y)));
      int d_ceil = static_cast<int>(std::ceil(disp_img(x,y)));
      int x_floor = x + d_floor, x_ceil = x + d_ceil;

      if( x_floor < 0 || x_ceil < 0 || x_floor >= w || x_ceil >= w )
        continue;

      // Check if either inverse disparity is consistent and flag if not.
      if( abs( inv_disp[x_floor] - d_floor ) > disp_thresh &&
        abs( inv_disp[x_ceil] - d_ceil ) > disp_thresh )
        disp_img(x,y) = invalid_disparity;
    } //x
  } //y

}


//-----------------------------------------------------------------
void bsgm_check_leftright(
  const vil_image_view<float>& disp1,
  const vil_image_view<float>& disp2,
  const vil_image_view<bool>& invalid1,
  const vil_image_view<bool>& invalid2,
  vil_image_view<bool>& error1)
{
  float dist_thresh = 1.0f;

  // Initialize error map
  error1.set_size(disp1.ni(), disp1.nj());
  error1.fill(true);

  // Iterate through first image
  for (int y = 0; y < static_cast<int>(disp1.nj()); y++) {
    for (int x = 0; x < static_cast<int>(disp1.ni()); x++) {
      if (invalid1(x, y)) continue;

      // Project each pixel into the second image
      int x2 = x + static_cast<int>(std::round(disp1(x, y) ));
      if (x2 < 0 || x2 >= static_cast<int>(disp2.ni())) continue;
      if (invalid2(x2, y)) continue;

      // Project back into first image and compute distance
      float x1 = x2 + disp2(x2, y);
      float dist = x1 - (float)x;
      if (fabs(dist) <= dist_thresh) error1(x, y) = false;
    }
  }
}


//-----------------------------------------------------------------------
void
bsgm_interpolate_errors(
  vil_image_view<float>& disp_img,
  const vil_image_view<bool>& invalid,
  const vil_image_view<vxl_byte>& img,
  float invalid_disparity,
  vxl_byte shadow_thresh)
{
  int num_sample_dirs = 8;
  float sample_percentile = 0.5f;
  float shadow_sample_percentile = 0.75f;

  int w = disp_img.ni(), h = disp_img.nj();
  std::vector<float> sample_vol( w*h*num_sample_dirs, 0.0f );
  vil_image_view<vxl_byte> sample_count( w, h );
  sample_count.fill( 0 );

  // Setup buffers
  std::vector<float> dir_sample_cur( w, 0.0f );
  std::vector<float> dir_sample_prev( w, 0.0f );

  // The following directional smoothing is adapted from run_multi_dp
  for( int dir = 0; dir < num_sample_dirs; dir++ ){

    int dx, dy, temp_dx = 0, temp_dy = 0;
    int x_start, y_start, x_end, y_end;

    // - - -
    // X X X
    // - - -
    if( dir == 0 ){
      dx = -1; dy = 0;
      x_start = 1; x_end = w-1;
      y_start = 0; y_end = h-1;

    } else if( dir == 1 ){
      dx = 1; dy = 0;
      x_start = w-2; x_end = 0;
      y_start = h-1; y_end = 0;

    // X - -
    // - X -
    // - - X
    } else if( dir == 2 ){
      dx = -1; dy = -1;
      x_start = 1; x_end = w-1;
      y_start = 1; y_end = h-1;

    } else if( dir == 3 ){
      dx = 1; dy = 1;
      x_start = w-2; x_end = 0;
      y_start = h-2; y_end = 0;

    // - X -
    // - X -
    // - X -
    } else if( dir == 4 ){
      dx = 0; dy = -1;
      x_start = 0; x_end = w-1;
      y_start = 1; y_end = h-1;

    } else if( dir == 5 ){
      dx = 0; dy = 1;
      x_start = w-1; x_end = 0;
      y_start = h-2; y_end = 0;

    // - - X
    // - X -
    // X - -
    } else if( dir == 6 ){
      dx = 1; dy = -1;
      x_start = w-2; x_end = 0;
      y_start = 1; y_end = h-1;

    } else if( dir == 7 ){
      dx = -1; dy = 1;
      x_start = 1; x_end = w-1;
      y_start = h-2; y_end = 0;
    }

    // Automatically determine iteration direction from end points
    int x_inc = (x_start < x_end) ? 1 : -1;
    int y_inc = (y_start < y_end) ? 1 : -1;

    // Initialize previous row
    for( int v = 0; v < w; v++ )
      dir_sample_prev[v] = invalid_disparity;

    // Loop through rows
    for( int y = y_start; y != y_end + y_inc; y += y_inc ){

      // Re-initialize current row in case dir follows row
      for( int x = 0; x < w; x++ )
        dir_sample_cur[x] = invalid_disparity;

      for( int x = x_start; x != x_end + x_inc; x += x_inc ){

        // If good sample at this pixel, record it
        if (std::isnan(invalid_disparity)) {
          if (std::isnan(disp_img(x, y)))
            continue;
        }
        else if( disp_img(x,y) != invalid_disparity ){
          dir_sample_cur[x] = disp_img(x,y);

        } else {

          // Otherwise propagate previous sample
          if( dy == 0 )
            dir_sample_cur[x] = dir_sample_cur[x+dx];
          else
            dir_sample_cur[x] = dir_sample_prev[x+dx];

          // And add sample to this pixel's sample set
          if (std::isnan(invalid_disparity)) {
            if (std::isnan(disp_img(x, y)))
              continue;
		  }
          if( dir_sample_cur[x] != invalid_disparity ){
            sample_vol[ num_sample_dirs*(y*w + x) + sample_count(x,y) ] =
              dir_sample_cur[x];
            sample_count(x,y)++;
          }
        }

      } //x

      // Copy current row to prev
      dir_sample_prev = dir_sample_cur;
    } //y
  }//dir

  // Iterpolate any invalid pixels by taking the median (or specified
  // percentile) of accumulated sample set.
  std::vector<float>::iterator sample_itr = sample_vol.begin();
  for( int y = 0; y < h; y++ ){
    for( int x = 0; x < w; x++, sample_itr += num_sample_dirs ){
      if (invalid(x, y)) continue;

      // Require half of the directions return valid samples, which prevents
      // bogus interpolation on the boundary of the disparity map
      if( sample_count(x,y) <= 4 ) continue;

      std::sort( sample_itr, sample_itr + sample_count(x,y) );

      int interp_idx;

      // Choose a sample index depending on whether pixel is shadow or not
      if (img(x, y) < shadow_thresh)
        interp_idx = static_cast<int>((shadow_sample_percentile*sample_count(x, y)));
      else
        interp_idx = static_cast<int>((sample_percentile*sample_count(x, y)));

      disp_img(x,y) = *( sample_itr + interp_idx );
    }
  }

}




//------------------------------------------------------------------------
void
bsgm_compute_invalid_map(
  const vil_image_view<vxl_byte>& img_tar,
  const vil_image_view<vxl_byte>& img_ref,
  vil_image_view<bool>& invalid_tar,
  int min_disparity,
  int num_disparities,
  vxl_byte border_val )
{
  int w = img_tar.ni(), h = img_tar.nj();

  invalid_tar.set_size( w, h );

  // Initialize map
  for( int y = 0; y < h; y++ )
    for( int x = 0; x < w; x++ )
      invalid_tar(x,y) = false;

   // Find the border in the target image
  for( int y = 0; y < h; y++ ){

    // Find the left border
    for( int x = 0; x < w; x++ ){
      invalid_tar(x,y) = true;
      if( img_tar(x,y) != border_val )
        break;
    } //x

    // Find the right border
    for( int x = w-1; x >= 0; x-- ){
      invalid_tar(x,y) = true;
      if( img_tar(x,y) != border_val )
        break;
    } //x
  } //y

  int max_disparity = min_disparity + num_disparities;

  // Find the border in the reference image
  for( int y = 0; y < h; y++ ){

    // Find the left border
    int lb = 0;
    for( int x = 0; x < w; x++, lb++ )
      if( img_ref(x,y) != border_val )
        break;

    // Mask any pixels in the target image which map into the left border
    for( int x = 0; x < std::min( w, lb - min_disparity ); x++ )
      invalid_tar(x,y) = true;

    // Find the right border
    int rb = w-1;
    for( int x = w-1; x >= 0; x--, rb-- )
      if( img_ref(x,y) != border_val )
        break;

    // Mask any pixels in the target image which map into the right border
    for( int x = std::max( 0, rb - max_disparity ); x < w ; x++ )
      invalid_tar(x,y) = true;
  } //y

  //vil_image_view<vxl_byte> vis( w_, h_ );
  //for( int y = 0; y < h_; y++ )
  //  for( int x = 0; x < w_; x++ )
  //    vis(x,y) = invalid_tar(x,y) ? 255 : 0;
  //vil_save( vis, "D:/results/sattel/invalid.png" );
}


//-----------------------------------------------------------------------
void
bsgm_invert_disparities(
  vil_image_view<float>& disp_img,
  int old_invalid,
  int new_invalid )
{
  for( int y = 0; y < static_cast<int>(disp_img.nj()); y++ )
    for( int x = 0; x < static_cast<int>(disp_img.ni()); x++ )
      disp_img(x,y) = disp_img(x,y) == old_invalid ? new_invalid : -disp_img(x,y);
}
