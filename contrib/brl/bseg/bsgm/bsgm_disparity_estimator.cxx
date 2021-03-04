// This is brl/bseg/bsgm/bsgm_disparity_estimator.cxx

#include <iomanip>
#include <algorithm>

#include "vil/vil_save.h"
#include "vil/vil_convert.h"


#include "bsgm_disparity_estimator.h"




//----------------------------------------------------------------------------
bsgm_disparity_estimator::bsgm_disparity_estimator(
  const bsgm_disparity_estimator_params& params,
  long long int cost_volume_width,
  long long int cost_volume_height,
  long long int num_disparities) :
    params_( params ),
    w_( cost_volume_width ),
    h_( cost_volume_height ),
    num_disparities_( num_disparities ),
    cost_unit_( 64 ),
    p1_base_( 1.0f ),
    p2_min_base_( 1.0f ),
    p2_max_base_( 8.0f )
{
  // Validate inputs
  if (cost_volume_width < 0 || cost_volume_height < 0 || num_disparities < 0) {
    std::ostringstream buffer;
    buffer << "Cannot construct bsgm_disparity_estimator with negative width, height, or num_disparities." << std::endl
           << "width = " << cost_volume_width << std::endl
           << "height = " << cost_volume_height << std::endl
           << "num_disparities = " << num_disparities << std::endl;
    throw std::runtime_error(buffer.str());
  }

  // Check cost volume size
  long long int cost_volume_size = cost_volume_width * cost_volume_height * num_disparities;
  if (cost_volume_size < static_cast<long long int>(0) ||
      cost_volume_size > static_cast<long long int>(total_cost_data_.max_size())) {
    std::ostringstream buffer;
    buffer << "Cannot construct bsgm_disparity_estimator - cost volume is too large." << std::endl
           << "width = " << cost_volume_width << std::endl
           << "height = " << cost_volume_height << std::endl
           << "num_disparities = " << num_disparities << std::endl;
    throw std::runtime_error(buffer.str());
  }

  // Setup any necessary cost volumes
  setup_cost_volume( fused_cost_data_, fused_cost_, num_disparities );
  setup_cost_volume( total_cost_data_, total_cost_, num_disparities );
}


//----------------------------------------------------------------------------
bsgm_disparity_estimator::~bsgm_disparity_estimator()
= default;

//----------------------------------------------------------------------------

void bsgm_disparity_estimator::compute_xgrad_data(
  const vil_image_view<float>& grad_x_tar,
  const vil_image_view<float>& grad_x_ref,
  const vil_image_view<bool>& invalid_tar,
  std::vector< std::vector< unsigned char* > >& app_cost,
  const vil_image_view<int>& min_disparity,
  const vgl_box_2d<int>& target_window)
{
  // target and reference images have same size
  int ni = static_cast<int>(grad_x_tar.ni()), nj = static_cast<int>(grad_x_tar.nj());

  // keep track of what windowed pixels in the full images the SGM volume corresponds to
  // note: have to do this because the target and reference windows will be different sizes,
  // so just simply cropping the gradient doesn't work
  int img_start_x, img_start_y;
  if (target_window.is_empty()) {
    img_start_x = 0;
    img_start_y = 0;
  }
  else {
    img_start_x = target_window.min_x();
    img_start_y = target_window.min_y();
  }

  float grad_norm = cost_unit_ / 8.0f;
  // Compute the appearance cost volume
  // (keep track of SGM cost volume indices, and the corresponding target image indices)
  for (int cost_y = 0, img_y = img_start_y; cost_y < h_; cost_y++, img_y++) {
    for (int cost_x = 0, img_x = img_start_x; cost_x < w_; cost_x++, img_x++) {

      unsigned char* ac = app_cost[cost_y][cost_x];

      // If invalid pixel, fill with 255
      if (invalid_tar(cost_x, cost_y)) {
        for (int d = 0; d < num_disparities_; d++, ac++)
          *ac = 255;
        continue;
      }

      // Otherwise compute all costs
      int img_x2 = img_x + min_disparity(cost_x, cost_y);
      for (int d = 0; d < num_disparities_; d++, img_x2++, ac++) {

        // Check valid match pixel
        /* if (img_x2 < 0 || img_x2 >= w_) */
        if (img_x2 < 0 || img_x2 >= ni)
          *ac = 255;

        // Compare gradient intensities
        else {

          // gradient comparison
          float g = grad_norm * fabs(grad_x_tar(img_x, img_y)
                                     - grad_x_ref(img_x2, img_y));

          // weighted update of appearance cost
          float ac_new = (float)(*ac) + params_.xgrad_weight * g;
          *ac = (unsigned char)(ac_new > 255.0f ? 255.0f : ac_new);
        }

      } //d
    } //j
  } //i

}



//----------------------------------------------------------------------------
void
bsgm_disparity_estimator::write_cost_debug_imgs(
  const std::string& out_dir,
  bool write_total_cost )
{
  if( active_app_cost_->size() == 0 ) return;

  // total cost maximum
  float total_cost_scale = 1.0f;
  if (write_total_cost) {
    auto max_total_cost = *std::max_element(total_cost_data_.begin(), total_cost_data_.end());
    total_cost_scale = 255.0f / float(max_total_cost);
    // std::cout << "MAX TOTAL COST " << max_total_cost << std::endl
    //           << "TOTAL COST SCALE " << total_cost_scale << std::endl;
  }

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
  long long int depth )
{
  cost_data.resize( w_*h_*depth );
  cost.resize( h_ );

  long long int idx = 0;
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
  long long int depth )
{
  cost_data.resize( w_*h_*depth );
  cost.resize( h_ );

  long long int idx = 0;
  for( int y = 0; y < h_; y++ ){
    cost[y].resize( w_ );
    for( int x = 0; x < w_; x++, idx += depth )
      cost[y][x] = &cost_data[idx];
  }
}

//-------------------------------------------------------------------------
void
bsgm_disparity_estimator::run_multi_dp(
  const std::vector< std::vector<unsigned char*> >&  /*app_cost*/,
  std::vector< std::vector<unsigned short*> >& total_cost,
  const vil_image_view<bool>& invalid_tar,
  const vil_image_view<float>& grad_x,
  const vil_image_view<float>& grad_y,
  const vil_image_view<int>& min_disparity)
{
  long long int volume_size = w_*h_*num_disparities_;
  long long int row_size = w_*num_disparities_;
  int num_dirs = params_.use_16_directions ? 16 : 8;
  float sqrt2norm = 1.0f/sqrt(2.0f);
  float grad_norm = 1.0f/params_.max_grad;

  auto bias_weight = params_.bias_weight;
  auto bias_dir = normalize(params_.bias_dir);
  bool using_bias = bias_weight > 0.0f;

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
    //if( dir != 3 ) continue;

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

    // Calculate directional weight if necessary
    float dir_weight = 1.0f;
    if (using_bias) {
      vgl_vector_2d<float> dp_dir((float)dx, -(float)dy);
      dp_dir = normalize(dp_dir);
      float cosa = dot_product(dp_dir, bias_dir);
      dir_weight = 1.0f - bias_weight*0.5f*(1.0f - cosa);
    }

    // Automatically determine iteration direction from end points
    int x_inc = (x_start < x_end) ? 1 : -1;
    int y_inc = (y_start < y_end) ? 1 : -1;

    // Initialize previous row
    for( long long int v = 0; v < row_size; v++ )
      dir_cost_prev[v] = 0;

    // Loop through rows
    for( int y = y_start; y != y_end + y_inc; y += y_inc ){

      // Re-initialize current row in case dir follows row
      for( long long int v = 0; v < row_size; v++ )
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
            total_cost[y][x], dir_weight*p1, dir_weight*p2,// p1, p2,
            min_disparity(x+dx,y+dy), min_disparity(x,y) );
        else
          compute_dir_cost(
            &dir_cost_prev[(x+dx)*num_disparities_],
            (*active_app_cost_)[y][x],
            &dir_cost_cur[x*num_disparities_],
            total_cost[y][x], dir_weight*p1, dir_weight*p2,// p1, p2,
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



//----------------------------------------------------------------------
vgl_box_2d<int>
bsgm_disparity_estimator::add_margin_to_window(
  const vgl_box_2d<int>& target_window,
  int margin,
  int ni,
  int nj)
{
  int minx = target_window.min_x() - margin;
  int miny = target_window.min_y() - margin;
  int maxx = target_window.max_x() + margin;
  int maxy = target_window.max_y() + margin;

  // clip to image bounds (end of window exclusive)
  minx = std::max(minx, 0);
  miny = std::max(miny, 0);
  maxx = std::min(maxx, ni);
  maxy = std::min(maxy, nj);

  return vgl_box_2d<int>(minx, maxx, miny, maxy);
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



//-----------------------------------------------------------------------
// output parameters
std::ostream& operator<<(std::ostream& os, const bsgm_disparity_estimator_params& params)
{
  os << "BSGM Disparity Estimator Parameters:" << std::endl
     << "use_16_directions:               " << params.use_16_directions << std::endl
     << "p1_scale:                        " << params.p1_scale << std::endl
     << "p2_scale:                        " << params.p2_scale << std::endl
     << "use_gradient_weighted_smoothing: " << params.use_gradient_weighted_smoothing << std::endl
     << "max_grad:                        " << params.max_grad << std::endl
     << "perform_quadratic_interp:        " << params.perform_quadratic_interp << std::endl
     << "error_check_mode:                " << params.error_check_mode << std::endl
     << "shadow_thresh:                   " << static_cast<unsigned>(params.shadow_thresh) << std::endl
     << "bias_weight:                     " << params.bias_weight << std::endl
     << "bias_dir:                        " << params.bias_dir << std::endl
     << "census_weight:                   " << params.census_weight << std::endl
     << "xgrad_weight:                    " << params.xgrad_weight << std::endl
     << "census_tol:                      " << params.census_tol << std::endl
     << "census_rad:                      " << params.census_rad << std::endl
     << "print_timing:                    " << params.print_timing << std::endl
     ;
  return os;
}
