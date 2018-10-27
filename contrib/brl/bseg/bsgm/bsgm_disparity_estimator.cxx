// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

#include <iomanip>
#include <algorithm>

#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_gauss_reduce.h>

#include "bsgm_disparity_estimator.h"
#include "bsgm_census.h"


//----------------------------------------------------------------------------
bsgm_disparity_estimator::bsgm_disparity_estimator(
  const bsgm_disparity_estimator_params& params,
  int img_width,
  int img_height,
  int num_disparities ) :
    params_( params ),
    w_( img_width ),
    h_( img_height ),
    num_disparities_( num_disparities ),
    cost_unit_( 64 ),
    p1_base_( 1.0f ),
    p2_min_base_( 1.0f ),
    p2_max_base_( 8.0f )
{
  // Setup any necessary cost volumes
  if( params_.census_weight > 0.0f )
    setup_cost_volume( census_cost_data_, census_cost_, num_disparities );
  if( params_.xgrad_weight > 0.0f )
    setup_cost_volume( xgrad_cost_data_, xgrad_cost_, num_disparities );
  if( params_.census_weight > 0.0f && params_.xgrad_weight > 0.0f )
    setup_cost_volume( fused_cost_data_, fused_cost_, num_disparities );
  setup_cost_volume( total_cost_data_, total_cost_, num_disparities );
}


//----------------------------------------------------------------------------
bsgm_disparity_estimator::~bsgm_disparity_estimator()
= default;


//----------------------------------------------------------------------------
bool
bsgm_disparity_estimator::compute(
  const vil_image_view<vxl_byte>& img_tar,
  const vil_image_view<vxl_byte>& img_ref,
  const vil_image_view<bool>& invalid_tar,
  const vil_image_view<int>& min_disp,
  float invalid_disp,
  vil_image_view<float>& disp_tar )
{
  disp_tar.set_size( w_, h_ );

  // Validate images.
  if( img_tar.ni() != w_ || img_tar.nj() != h_ ||
      img_ref.ni() != w_ || img_ref.nj() != h_ ||
      invalid_tar.ni() != w_ || invalid_tar.nj() != h_ ) return false;

  int num_voxels = w_*h_*num_disparities_;

  vul_timer timer, total_timer;
  if( params_.print_timing ){
    timer.mark(); total_timer.mark();
  }

  // Compute gradient images.
  vil_image_view<float> grad_x_tar, grad_y_tar, grad_x_ref, grad_y_ref;
  if( params_.use_gradient_weighted_smoothing ||
      params_.xgrad_weight > 0.0f )
    vil_sobel_3x3<vxl_byte,float>( img_tar, grad_x_tar, grad_y_tar );

  if( params_.print_timing )
    print_time( "Gradient image computation", timer );

  // Compute appearance cost volume data.
  if( params_.census_weight > 0.0f ){
    active_app_cost_ = &census_cost_;

    compute_census_data( img_tar, img_ref,
      invalid_tar, census_cost_, min_disp );
  }
  if( params_.xgrad_weight > 0.0f ){
    active_app_cost_ = &xgrad_cost_;

    vil_sobel_3x3<vxl_byte,float>( img_ref, grad_x_ref, grad_y_ref );
    compute_xgrad_data( grad_x_tar, grad_x_ref,
      invalid_tar, xgrad_cost_, min_disp );
  }

  if( params_.print_timing )
    print_time( "Appearance cost computation", timer );

  // Fuse appearance volumes if necessary.
  if( params_.census_weight > 0.0f && params_.xgrad_weight > 0.0f ){
    active_app_cost_ = &fused_cost_;

    for( int v = 0; v < num_voxels; v++ ){
      float fc = params_.census_weight*census_cost_data_[v] +
        params_.xgrad_weight*xgrad_cost_data_[v];
      fused_cost_data_[v] = (unsigned char)( fc < 255.0f ? fc : 255.0f );
    }

    if( params_.print_timing )
      print_time( "Appearance fusion", timer );
  }

  // Run the multi-directional dynamic programming to obtain a total cost
  // volume incorporating appearance + smoothing.
  run_multi_dp(
    *active_app_cost_, total_cost_,
    invalid_tar, grad_x_tar, grad_y_tar, min_disp );

  if( params_.print_timing )
    print_time( "Dynamic programming", timer );

  // Find the lowest total cost disparity for each pixel, do quadratic
  // interpolation if configured.
  vil_image_view<unsigned short> disp_cost ;
  compute_best_disparity_img( total_cost_, min_disp,
    invalid_tar, invalid_disp, disp_tar, disp_cost );

  // Median filter to remove speckles
  vil_structuring_element se; se.set_to_disk( 1.9 );
  vil_image_view<float> disp2( w_, h_ );
  vil_median( disp_tar, disp2, se );
  disp_tar.deep_copy( disp2 );

  if( params_.print_timing )
    print_time( "Disparity map extraction", timer );

  // Find and fix errors if configured.
  if( params_.error_check_mode > 0 ){
    flag_nonunique( disp_tar, disp_cost, img_tar, invalid_disp );

    if( params_.error_check_mode > 1 )
      interpolate_errors( disp_tar, invalid_tar, img_tar, invalid_disp );
  }

  if( params_.print_timing )
    print_time( "Consistency check", timer );

  if( params_.print_timing )
    print_time( "TOTAL TIME", total_timer );

  return true;
}


//----------------------------------------------------------------------------
void
bsgm_disparity_estimator::write_cost_debug_imgs(
  const std::string& out_dir,
  bool write_total_cost )
{
  float total_cost_scale = 0.25f;

  if( active_app_cost_->size() == 0 ) return;

  vil_image_view<vxl_byte> vis_img( w_, h_ );

  for( int d = 0; d < num_disparities_; d++ ){

    // Gather costs for each disparity slice
    if( write_total_cost ){
      for( int y = 0; y < h_; y++ )
        for( int x = 0; x < w_; x++ )
          vis_img(x,y) = (unsigned char)std::min( 255.0f,
            total_cost_scale*total_cost_[y][x][d] );

    } else {
      for( int y = 0; y < h_; y++ )
        for( int x = 0; x < w_; x++ )
          vis_img(x,y) = (*active_app_cost_)[y][x][d];
    }

    // Write slice image
    std::stringstream dss;
    dss << out_dir << '/'
      << std::setfill( '0' ) << std::setw(3) << d << ".png";
    vil_save( vis_img, dss.str().c_str() );
  }
}



//------------------------------------------------------------------------
void
bsgm_disparity_estimator::setup_cost_volume(
  std::vector<unsigned char>& cost_data,
  std::vector< std::vector< unsigned char* > >& cost,
  int depth )
{
  cost_data.resize( w_*h_*depth );
  cost.resize( h_ );

  int idx = 0;
  for( int y = 0; y < h_; y++ ){
    cost[y].resize( w_ );
    for( int x = 0; x < w_; x++, idx += depth )
      cost[y][x] = &cost_data[idx];
  }
}


//------------------------------------------------------------------------
void
bsgm_disparity_estimator::setup_cost_volume(
  std::vector<unsigned short>& cost_data,
  std::vector< std::vector< unsigned short* > >& cost,
  int depth )
{
  cost_data.resize( w_*h_*depth );
  cost.resize( h_ );

  int idx = 0;
  for( int y = 0; y < h_; y++ ){
    cost[y].resize( w_ );
    for( int x = 0; x < w_; x++, idx += depth )
      cost[y][x] = &cost_data[idx];
  }
}


//-----------------------------------------------------------------------
void
bsgm_disparity_estimator::compute_census_data(
  const vil_image_view<vxl_byte>& img_tar,
  const vil_image_view<vxl_byte>& img_ref,
  const vil_image_view<bool>& invalid_tar,
  std::vector< std::vector< unsigned char* > >& app_cost,
  const vil_image_view<int>& min_disparity )
{
  int census_diam = 2*params_.census_rad + 1;
  if( census_diam > 7 ) census_diam = 7;
  if( census_diam < 3 ) census_diam = 3;
  float census_norm = 8.0f*cost_unit_/(float)(census_diam*census_diam);
  bool only_32_bits = census_diam <= 5;

  // Compute census images
  vil_image_view<vxl_uint_64> census_tar, census_ref;
  vil_image_view<vxl_uint_64> census_conf_tar, census_conf_ref;
  bsgm_compute_census_img(
    img_tar, census_diam, census_tar, census_conf_tar, params_.census_tol );
  bsgm_compute_census_img(
    img_ref, census_diam, census_ref, census_conf_ref, params_.census_tol );

  // Construct a bit-set look-up table for use later
  unsigned char bit_set_table[256];
  bsgm_generate_bit_set_lut( bit_set_table );

  // Compute the appearance cost volume
  for( int y = 0; y < h_; y++ ){
    for( int x = 0; x < w_; x++ ){

      // If invalid pixel, fill with 255
      if( invalid_tar(x,y) ){
        for( int d = 0; d < num_disparities_; d++ )
          app_cost[y][x][d] = 255;
        continue;
      }

      // Start iterating through disparities
      int d = 0;
      int x2 = x + min_disparity(x,y);

      // Pixels off left-side of image set to 255
      unsigned char* ac = &app_cost[y][x][0];
      for( ; x2 < 0; d++, x2++, ac++ )
        *ac = 255;

      // This needed in rare case that min_disparity > 0
      for( ; x2 >= w_ && d < num_disparities_; d++, x2++, ac++ )
        *ac = 255;

      // Compute census costs for all valid disparities
      vxl_uint_64 cen_t = census_tar(x,y);
      vxl_uint_64* cen_r = &census_ref(x2,y);
      vxl_uint_64 conf_t = census_conf_tar(x,y);
      vxl_uint_64* conf_r = &census_conf_ref(x2,y);

      for( ; d < num_disparities_; d++, x2++, ac++, cen_r++, conf_r++ ){

        // Check valid match pixel
        if( x2 >= w_ )
          *ac = 255;

        // Compare census images using hamming distance
        else {

          unsigned long long int cen_diff =
            bsgm_compute_diff_string( cen_t, *cen_r, conf_t, *conf_r );

          unsigned char ham =
            bsgm_compute_hamming_lut( cen_diff, bit_set_table, only_32_bits );

          float ham_norm = census_norm*ham;
          *ac = (unsigned char)( ham_norm > 255.0f ? 255.0f : ham_norm );
        }

      } //d
    } //j
  } //i
}


//----------------------------------------------------------------------------
void
bsgm_disparity_estimator::compute_xgrad_data(
  const vil_image_view<float>& grad_x_tar,
  const vil_image_view<float>& grad_x_ref,
  const vil_image_view<bool>& invalid_tar,
  std::vector< std::vector< unsigned char* > >& app_cost,
  const vil_image_view<int>& min_disparity )
{
  float grad_norm = cost_unit_/8.0f;

  // Compute the appearance cost volume
  for( int y = 0; y < h_; y++ ){
    for( int x = 0; x < w_; x++ ){

      unsigned char* ac = app_cost[y][x];

      // If invalid pixel, fill with 255
      if( invalid_tar(x,y) ){
        for( int d = 0; d < num_disparities_; d++, ac++ )
          *ac = 255;
        continue;
      }

      // Otherwise compute all costs
      int x2 = x + min_disparity(x,y);
      for( int d = 0; d < num_disparities_; d++, x2++, ac++ ){

        // Check valid match pixel
        if( x2 < 0 || x2 >= w_ )
          *ac = 255;

        // Compare gradient intensities
        else {
          float g = grad_norm*fabs( grad_x_tar(x,y) - grad_x_ref(x2,y) );
          *ac = (unsigned char)( g < 255.0f ? g : 255.0f );
        }
      } //d
    } //j
  } //i

}


//-------------------------------------------------------------------------
void
bsgm_disparity_estimator::run_multi_dp(
  const std::vector< std::vector<unsigned char*> >&  /*app_cost*/,
  std::vector< std::vector<unsigned short*> >& total_cost,
  const vil_image_view<bool>& invalid_tar,
  const vil_image_view<float>& grad_x,
  const vil_image_view<float>& grad_y,
  const vil_image_view<int>& min_disparity )
{
  int volume_size = w_*h_*num_disparities_;
  int row_size = w_*num_disparities_;
  int num_dirs = params_.use_16_directions ? 16 : 8;
  float sqrt2norm = 1.0f/sqrt(2.0f);
  float grad_norm = 1.0f/params_.max_grad;

  // Compute directional derivatives used for gradient-weighted smoothing
  std::vector< vil_image_view<float> > deriv_img(4);
  if( params_.use_gradient_weighted_smoothing ){

    for( int g = 0; g < 4; g++ )
      deriv_img[g] = vil_image_view<float>( w_, h_ );

    for( int y = 0; y < h_; y++ ){
      for( int x = 0; x < w_; x++ ){
        float g0 = fabs( grad_x(x,y) )*grad_norm;
        deriv_img[0](x,y) = g0 < 1.0f ? g0 : 1.0f;
        float g1 = fabs( grad_y(x,y) )*grad_norm;
        deriv_img[1](x,y) = g1 < 1.0f ? g1 : 1.0f;
        float g2 = fabs( sqrt2norm*( grad_x(x,y) + grad_y(x,y) ) )*grad_norm;
        deriv_img[2](x,y) = g2 < 1.0f ? g2 : 1.0f;
        float g3 = fabs( sqrt2norm*( grad_x(x,y) - grad_y(x,y) ) )*grad_norm;
        deriv_img[3](x,y) = g3 < 1.0f ? g3 : 1.0f;
      }
    }
  }

  //vil_image_view<vxl_byte> vis;
  //vil_convert_stretch_range_limited( grad_x, vis, -60.0f, 60.0f );
  //vil_save( vis, "D:/results/a.png" );

  // These will be default P1, P2 costs if no gradient-weighted smoothing
  auto p1 = (unsigned short)( p1_base_*cost_unit_*params_.p1_scale );
  float p2_max = p2_max_base_*cost_unit_*params_.p2_scale;
  float p2_min = p2_min_base_*cost_unit_*params_.p2_scale;
  auto p2 = (unsigned short)( p2_max );

  // Initialize total cost
  for( int y = 0; y < h_; y++ )
    for( int x = 0; x < w_; x++ )
      for( int d = 0; d < num_disparities_; d++ )
        total_cost[y][x][d] = 0;

  // Setup buffers
  std::vector<unsigned short> dir_cost_cur( row_size, (unsigned short)0 );
  std::vector<unsigned short> dir_cost_prev( row_size, (unsigned short)0 );

  // Compute the smoothing costs for each direction independently
  for( int dir = 0; dir < num_dirs; dir++ ){

    // HACK HERE TO RUN A SINGLE DIRECTION
    //if( dir != 15 ) continue;

    // The 8 or 16 dynamic programming directions are set in the following
    // section.  Each even/odd dir index pair correspond to the same path but
    // in reverse.
    int dx, dy, temp_dx = 0, temp_dy = 0;
    int x_start, y_start, x_end, y_end;
    bool alt_x = false, alt_y = false;
    int deriv_idx;

    // - - -
    // X X X
    // - - -
    if( dir == 0 ){
      dx = -1; dy = 0;
      x_start = 1; x_end = w_-1;
      y_start = 0; y_end = h_-1;
      deriv_idx = 0;

    } else if( dir == 1 ){
      dx = 1; dy = 0;
      x_start = w_-2; x_end = 0;
      y_start = h_-1; y_end = 0;
      deriv_idx = 0;

    // X - -
    // - X -
    // - - X
    } else if( dir == 2 ){
      dx = -1; dy = -1;
      x_start = 1; x_end = w_-1;
      y_start = 1; y_end = h_-1;
      deriv_idx = 2;

    } else if( dir == 3 ){
      dx = 1; dy = 1;
      x_start = w_-2; x_end = 0;
      y_start = h_-2; y_end = 0;
      deriv_idx = 2;

    // - X -
    // - X -
    // - X -
    } else if( dir == 4 ){
      dx = 0; dy = -1;
      x_start = 0; x_end = w_-1;
      y_start = 1; y_end = h_-1;
      deriv_idx = 1;

    } else if( dir == 5 ){
      dx = 0; dy = 1;
      x_start = w_-1; x_end = 0;
      y_start = h_-2; y_end = 0;
      deriv_idx = 1;

    // - - X
    // - X -
    // X - -
    } else if( dir == 6 ){
      dx = 1; dy = -1;
      x_start = w_-2; x_end = 0;
      y_start = 1; y_end = h_-1;
      deriv_idx = 3;

    } else if( dir == 7 ){
      dx = -1; dy = 1;
      x_start = 1; x_end = w_-1;
      y_start = h_-2; y_end = 0;
      deriv_idx = 3;

    // X - -      - - -
    // - X X  or  X X -
    // - - -      - - X
    } else if( dir == 8 ){
      dx = -1; dy = -1; alt_y = true;
      x_start = 1; x_end = w_-1;
      y_start = 1; y_end = h_-1;
      deriv_idx = 0;

    } else if( dir == 9 ){
      dx = 1; dy = 1; alt_y = true;
      x_start = w_-2; x_end = 0;
      y_start = h_-2; y_end = 0;
      deriv_idx = 0;

    // - X -      X - -
    // - X -  or  - X -
    // - - X      - X -
    } else if( dir == 10 ){
      dx = -1; dy = -1; alt_x = true;
      x_start = 1; x_end = w_-1;
      y_start = 1; y_end = h_-1;
      deriv_idx = 1;

    } else if( dir == 11 ){
      dx = 1; dy = 1; alt_x = true;
      x_start = w_-2; x_end = 0;
      y_start = h_-2; y_end = 0;
      deriv_idx = 1;

    // - X -      - - X
    // - X -  or  - X -
    // X - -      - X -
    } else if( dir == 12 ){
      dx = 1; dy = -1; alt_x = true;
      x_start = w_-2; x_end = 0;
      y_start = 1; y_end = h_-1;
      deriv_idx = 1;

    } else if( dir == 13 ){
      dx = -1; dy = 1; alt_x = true;
      x_start = 1; x_end = w_-1;
      y_start = h_-2; y_end = 0;
      deriv_idx = 1;

    // - - X      - - -
    // X X -  or  - X X
    // - - -      X - -
    } else if( dir == 14 ){
      dx = 1; dy = -1; alt_y = true;
      x_start = w_-2; x_end = 0;
      y_start = 1; y_end = h_-1;
      deriv_idx = 0;

    } else if( dir == 15 ){
      dx = -1; dy = 1; alt_y = true;
      x_start = 1; x_end = w_-1;
      y_start = h_-2; y_end = 0;
      deriv_idx = 0;
    }

    // Automatically determine iteration direction from end points
    int x_inc = (x_start < x_end) ? 1 : -1;
    int y_inc = (y_start < y_end) ? 1 : -1;

    // Initialize previous row
    for( int v = 0; v < row_size; v++ )
      dir_cost_prev[v] = 0;

    // Loop through rows
    for( int y = y_start; y != y_end + y_inc; y += y_inc ){

      // Re-initialize current row in case dir follows row
      for( int v = 0; v < row_size; v++ )
        dir_cost_cur[v] = 0;

      // Swap path idx if necessary for directions 8-15
      if( alt_x ) std::swap( dx, temp_dx );
      if( alt_y && dy == 0 ) std::swap( dy, temp_dy );

      for( int x = x_start; x != x_end + x_inc; x += x_inc ){

        // Swap path idx if necessary for directions 8-15
        if( alt_y ) std::swap( dy, temp_dy );

        // Quit early if invalid pixel
        if( invalid_tar(x,y) )
          continue;

        // If configured, compute a P2 weight based on local gradient
        if( params_.use_gradient_weighted_smoothing )
          p2 = (unsigned short)(
            p2_max + (p2_min-p2_max)*deriv_img[deriv_idx](x,y) );

        // Compute the directional smoothing cost and add to total
        if( dy == 0 )
          compute_dir_cost(
            &dir_cost_cur[(x+dx)*num_disparities_],
            (*active_app_cost_)[y][x],
            &dir_cost_cur[x*num_disparities_],
            total_cost[y][x], p1, p2,
            min_disparity(x+dx,y+dy), min_disparity(x,y) );
        else
          compute_dir_cost(
            &dir_cost_prev[(x+dx)*num_disparities_],
            (*active_app_cost_)[y][x],
            &dir_cost_cur[x*num_disparities_],
            total_cost[y][x], p1, p2,
            min_disparity(x+dx,y+dy), min_disparity(x,y) );
      } //x

      // Copy current row to previous
      dir_cost_prev = dir_cost_cur;
    } //y
  } //dir

}//*/


//--------------------------------------------------------------------
void
bsgm_disparity_estimator::compute_dir_cost(
  const unsigned short* prev_row_cost,
  const unsigned char* cur_app_cost,
  unsigned short* cur_row_cost,
  unsigned short* total_cost,
  unsigned short p1,
  unsigned short p2,
  int prev_min_disparity,
  int cur_min_disparity )
{
  // Compute the offset the aligns previous and current disparities
  int prev_offset = cur_min_disparity - prev_min_disparity;

  const unsigned short* prc = prev_row_cost;

  // Compute jump cost from best previous disparity with p2 penalty
  unsigned short min_prev_cost = *prc; prc++;

  for( int d = 1; d < num_disparities_; d++, prc++ )
    min_prev_cost = *prc < min_prev_cost ? *prc : min_prev_cost;
  unsigned short jump_cost = min_prev_cost + p2;

  // Setup iteration pointers for coming loop
  prc = prev_row_cost + prev_offset;
  const unsigned char* cac = cur_app_cost;
  unsigned short* crc = cur_row_cost;
  unsigned short* tc = total_cost;

  // Main loop through disparities
  for( int d = 0; d < num_disparities_; d++, prc++, cac++, crc++, tc++ ){

    // This is the index of d in the previous cost vector
    int d_off = d + prev_offset;

    // The best cost for each disparity is the min of the jump with cost P2...
    unsigned short best_cost = jump_cost;

    // ...the min of no disparity change with 0 cost...
    if( d_off >= 0 && d_off < num_disparities_ ){
      unsigned short prc_d = *prc;
      best_cost = prc_d < best_cost ? prc_d: best_cost;
    }

    // ...and +/- 1 disparity with P1 cost
    if( d_off > 0 && d_off <= num_disparities_ ){
      unsigned short prc_dm1 = *(prc-1) + p1;
      best_cost = prc_dm1 < best_cost ? prc_dm1: best_cost;
    }
    if( d_off >= -1 && d_off < num_disparities_-1 ){
      unsigned short prc_dp1 = *(prc+1) + p1;
      best_cost = prc_dp1 < best_cost ? prc_dp1: best_cost;
    }

    // Add the appearance cost and subtract off lowest cost to prevent
    // numerical overflow
    *crc = *cac + best_cost - min_prev_cost;

    // Add current cost to total
    *tc += *crc;

  } //d

}


//-------------------------------------------------------------------
void
bsgm_disparity_estimator::compute_best_disparity_img(
  const std::vector< std::vector< unsigned short* > >& total_cost,
  const vil_image_view<int>& min_disparity,
  const vil_image_view<bool>& invalid_tar,
  float invalid_disparity,
  vil_image_view<float>& disp_img,
  vil_image_view<unsigned short>& disp_cost )
{
  disp_img.set_size( w_, h_ );
  disp_cost.set_size( w_, h_ );

  for( int y = 0; y < h_; y++ ){
    for( int x = 0; x < w_; x++ ){

      // Quit early for invalid pixel
      if( invalid_tar(x,y) ){
        disp_img(x,y) = invalid_disparity;
        disp_cost(x,y) = 65535;
        continue;
      }

      unsigned short min_cost = total_cost[y][x][0];
      int min_cost_idx = 0;

      // Find the min cost index for each pixel
      for( int d = 1; d < num_disparities_; d++ ){
        if( total_cost[y][x][d] < min_cost ){
          min_cost = total_cost[y][x][d];
          min_cost_idx = d;
        }
      }

      disp_cost(x,y) = min_cost;

      // If no quadratic interpolation just record the min index
      disp_img(x,y) = (float)min_cost_idx;

      // Use quadratic interpolation to obtain sub-pixel estimate if specified
      if( params_.perform_quadratic_interp ){

        // In rare case of min cost disparity at an end point, set disparity
        // to end point +/-0.5 to ensure a continuous range of disparities
        if( min_cost_idx == 0 )
          disp_img(x,y) = 0.5f;
        else if( min_cost_idx == num_disparities_-1 )
          disp_img(x,y) = num_disparities_-1.5f;

        // In the typical case pick cost samples on either side of the min
        // disparity, fit a quadratic, and solve for the min
        else {

          auto c1 = (float)total_cost[y][x][min_cost_idx-1];
          auto c2 = (float)total_cost[y][x][min_cost_idx];
          auto c3 = (float)total_cost[y][x][min_cost_idx+1];

          // This finds the min of the quadratic without explicitly computing
          // the whole quadratic.  Note the min will necessarily be within
          // +/- 0.5 of the integer minimum.
          float denom = c1 + c3 - (2*c2);
          if( denom > 0.0f  )
            disp_img(x,y) += (c1-c3)/(2*denom);
        }
      }

      // Add in the min disparity to make it an absolute disparity
      disp_img(x,y) += min_disparity(x,y);
    } //x
  } //y
}


//-----------------------------------------------------------------------------
void
bsgm_disparity_estimator::flag_nonunique(
  vil_image_view<float>& disp_img,
  const vil_image_view<unsigned short>& disp_cost,
  const vil_image_view<vxl_byte>& img,
  float invalid_disparity,
  int disp_thresh )
{
  std::vector<unsigned short> inv_cost( w_ );
  std::vector<int> inv_disp( w_ );

  for( int y = 0; y < h_; y++ ){

    // Initialize an inverse disparity map for this row
    for( int x = 0; x < w_; x++ ){
      inv_cost[x] = 65535;
      inv_disp[x] = -1;
    }

    // Construct the inverse disparity map
    for( int x = 0; x < w_; x++ ){

      if( disp_img(x,y) == invalid_disparity ) continue;

      // Get an integer disparity and location in reference image
      int d = (int)( disp_img(x,y)+0.5f );
      int x_l = x + d;

      if( x_l < 0 || x_l >= w_ ) continue;

      // Record the min cost pixel mapping back to x_l
      if( inv_cost[x_l] > disp_cost(x,y) ){
        inv_cost[x_l] = disp_cost(x,y);
        inv_disp[x_l] = d;
      }
    } //x

    // Check the uniqueness of each disparity.
    for( int x = 0; x < w_; x++ ){

      if( disp_img(x,y) == invalid_disparity ) continue;

      // Label dark pixels as invalid, if thresh=0 nothing happens
      if (img(x, y) < params_.shadow_thresh) {
        disp_img(x, y) = invalid_disparity;
        continue;
      }

      // Compute the floor and ceiling of each disparity
      int d_floor = (int)floor(disp_img(x,y));
      int d_ceil = (int)ceil(disp_img(x,y));
      int x_floor = x + d_floor, x_ceil = x + d_ceil;

      if( x_floor < 0 || x_ceil < 0 || x_floor >= w_ || x_ceil >= w_ )
        continue;

      // Check if either inverse disparity is consistent and flag if not.
      if( abs( inv_disp[x_floor] - d_floor ) > disp_thresh &&
        abs( inv_disp[x_ceil] - d_ceil ) > disp_thresh )
        disp_img(x,y) = invalid_disparity;
    } //x
  } //y

}


//-----------------------------------------------------------------------
void
bsgm_disparity_estimator::interpolate_errors(
  vil_image_view<float>& disp_img,
  const vil_image_view<bool>& invalid,
  const vil_image_view<vxl_byte>& img,
  float invalid_disparity )
{
  int num_sample_dirs = 8;
  float sample_percentile = 0.5f;
  float shadow_sample_percentile = 0.75f;

  std::vector<float> sample_vol( w_*h_*num_sample_dirs, 0.0f );
  vil_image_view<vxl_byte> sample_count( w_, h_ );
  sample_count.fill( 0 );

  // Setup buffers
  std::vector<float> dir_sample_cur( w_, 0.0f );
  std::vector<float> dir_sample_prev( w_, 0.0f );

  // The following directional smoothing is adapted from run_multi_dp
  for( int dir = 0; dir < num_sample_dirs; dir++ ){

    int dx, dy, temp_dx = 0, temp_dy = 0;
    int x_start, y_start, x_end, y_end;

    // - - -
    // X X X
    // - - -
    if( dir == 0 ){
      dx = -1; dy = 0;
      x_start = 1; x_end = w_-1;
      y_start = 0; y_end = h_-1;

    } else if( dir == 1 ){
      dx = 1; dy = 0;
      x_start = w_-2; x_end = 0;
      y_start = h_-1; y_end = 0;

    // X - -
    // - X -
    // - - X
    } else if( dir == 2 ){
      dx = -1; dy = -1;
      x_start = 1; x_end = w_-1;
      y_start = 1; y_end = h_-1;

    } else if( dir == 3 ){
      dx = 1; dy = 1;
      x_start = w_-2; x_end = 0;
      y_start = h_-2; y_end = 0;

    // - X -
    // - X -
    // - X -
    } else if( dir == 4 ){
      dx = 0; dy = -1;
      x_start = 0; x_end = w_-1;
      y_start = 1; y_end = h_-1;

    } else if( dir == 5 ){
      dx = 0; dy = 1;
      x_start = w_-1; x_end = 0;
      y_start = h_-2; y_end = 0;

    // - - X
    // - X -
    // X - -
    } else if( dir == 6 ){
      dx = 1; dy = -1;
      x_start = w_-2; x_end = 0;
      y_start = 1; y_end = h_-1;

    } else if( dir == 7 ){
      dx = -1; dy = 1;
      x_start = 1; x_end = w_-1;
      y_start = h_-2; y_end = 0;
    }

    // Automatically determine iteration direction from end points
    int x_inc = (x_start < x_end) ? 1 : -1;
    int y_inc = (y_start < y_end) ? 1 : -1;

    // Initialize previous row
    for( int v = 0; v < w_; v++ )
      dir_sample_prev[v] = invalid_disparity;

    // Loop through rows
    for( int y = y_start; y != y_end + y_inc; y += y_inc ){

      // Re-initialize current row in case dir follows row
      for( int x = 0; x < w_; x++ )
        dir_sample_cur[x] = invalid_disparity;

      for( int x = x_start; x != x_end + x_inc; x += x_inc ){

        // If good sample at this pixel, record it
        if( disp_img(x,y) != invalid_disparity ){
          dir_sample_cur[x] = disp_img(x,y);

        } else {

          // Otherwise propagate previous sample
          if( dy == 0 )
            dir_sample_cur[x] = dir_sample_cur[x+dx];
          else
            dir_sample_cur[x] = dir_sample_prev[x+dx];

          // And add sample to this pixel's sample set
          if( dir_sample_cur[x] != invalid_disparity ){
            sample_vol[ num_sample_dirs*(y*w_ + x) + sample_count(x,y) ] =
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
  auto sample_itr = sample_vol.begin();
  for( int y = 0; y < h_; y++ ){
    for( int x = 0; x < w_; x++, sample_itr += num_sample_dirs ){
      if( sample_count(x,y) == 0 ) continue;
      if( invalid(x,y) ) continue;

      std::sort( sample_itr, sample_itr + sample_count(x,y) );

      int interp_idx;

      // Choose a sample index depending on whether pixel is shadow or not
      if (img(x, y) < params_.shadow_thresh)
        interp_idx = (int)(shadow_sample_percentile*sample_count(x, y));
      else
        interp_idx = (int)(sample_percentile*sample_count(x, y));

      disp_img(x,y) = *( sample_itr + interp_idx );
    }
  }

}


//----------------------------------------------------------------------
void
bsgm_disparity_estimator::print_time(
  const char* name,
  vul_timer& timer )
{
  std::cerr << name << ": " << timer.real() << "ms\n";
  timer.mark();
}


//------------------------------------------------------------------------
void
compute_invalid_map(
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
  for( int y = 0; y < (int)disp_img.nj(); y++ )
    for( int x = 0; x < (int)disp_img.ni(); x++ )
      disp_img(x,y) = disp_img(x,y) == old_invalid ? new_invalid : -disp_img(x,y);
}
