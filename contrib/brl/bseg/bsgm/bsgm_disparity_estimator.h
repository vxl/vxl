// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef bsgm_disparity_estimator_h_
#define bsgm_disparity_estimator_h_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <utility>

#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vgl/vgl_vector_2d.h>
#include "bsgm_census.h"
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_median.h>
#include <vil/algo/vil_binary_erode.h>
#include <vil/algo/vil_gauss_reduce.h>
#include "bsgm_error_checking.h"
//:
// \file
// \brief An implementation of the semi-global matching stereo algorithm.
// \author Thomas Pollard
// \date April 17, 2016


//: A struct containing miscellaneous SGM parameters
struct bsgm_disparity_estimator_params
{
  //: Use 16 directions in the dynamic programming, otherwise 8.  This
  // roughly doubles computation time.
  bool use_16_directions;

  //: Scale the internally set P1, P2 smoothing parameters.
  float p1_scale;
  float p2_scale;

  //: Use gradient-weighted P2 smoothing, as suggested in paper.
  // since discontinuities in depth are often manifested as intensity discontinuities
  // the cost of a disparity change greater than 1 is lowered by dividing P2 by the
  // magnitude of the x gradient.
  bool use_gradient_weighted_smoothing;

  //: In gradient-weighted smoothing, gradients beyond this magnitude are
  // truncated.
  float max_grad;

  //: Use quadratic interpolation to obtain sub-pixel estimates of final
  // disparity map.
  bool perform_quadratic_interp;

  //: Mode for finding and fixing errors in the disparity map
  // 0 raw disparity map
  // 1 bad pixels flagged
  // 2 bad pixels interpolated over
  int error_check_mode;

  //: When set > 0, pixels below this threshold will be flagged as invalid
  // when error_check_mode > 0
  unsigned short shadow_thresh;

  //: Set "bias_weight" to the range (0.0,1.0] to bias the SGM directional average
  // against the "bias_dir" parameter.  Use this if smoothing from certain
  // directions (i.e. sun angle for satellite imagery) is unreliable.  Set to
  // 0 to disable biasing.
  float bias_weight;
  vgl_vector_2d<float> bias_dir;

  //: Appearance costs computed by different algorithms are statically fused
  // using these weights. Set any to <= 0 to prevent computation.
  float census_weight;
  float xgrad_weight;

  //: Pixel differences less than this magnitude are not considered in the
  // census computation.  Increase to prevent errors from sensor noise.
  // Set to 0 for textbook census implementation.
  int census_tol;

  //: The length of the census kernal will be 2*census_rad+1. Must be 1,2,or 3.
  int census_rad;

  //: Print detailed timing information to cerr.
  bool print_timing;

  //: Default parameters
  bsgm_disparity_estimator_params():
    use_16_directions(false),
    p1_scale(1.0f),
    p2_scale(1.0f),
    use_gradient_weighted_smoothing(true),
    max_grad(32.0f),
    perform_quadratic_interp(true),
    error_check_mode(1),
    shadow_thresh(0),
    bias_weight(0.0f),
    bias_dir(1.0f,0.0f),
    census_weight(0.3f),
    xgrad_weight(0.7f),
    census_tol(2),
    census_rad(2),
    print_timing(false){}

};

// output parameters
std::ostream& operator<<(std::ostream& os, const bsgm_disparity_estimator_params& params);


class bsgm_disparity_estimator
{
 public:

  //: Construct from parameters
  bsgm_disparity_estimator(
    const bsgm_disparity_estimator_params& params,
    long long int img_width,
    long long int img_height,
    long long int num_disparities);
    //: Destructor
  ~bsgm_disparity_estimator();

  //: Run the SGM algorithm to compute a disparity map for the img_target,
  // where disparities measure the displacement of the reference pixel from
  // the target pixel i.e.
  //   img_target(x,y) <-> img_ref( x + disp_target(x,y), y )
  // Note that this is the opposite of the OpenCV SGM implementation.
  template <class T>
  bool compute(
    const vil_image_view<T>& img_target,
    const vil_image_view<T>& img_ref,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<int>& min_disparity,
    float invalid_disparity,
    vil_image_view<float>& disp_target,
    float dynamic_range_factor = 1.0f,
    bool skip_error_check = false);

  //: Write out the appearance or total cost volume as a set of images for
  // debugging
  void write_cost_debug_imgs(
    const std::string& out_dir,
    bool write_total_cost = false );

 protected:

  //: Copy of the input params
  bsgm_disparity_estimator_params params_;

  //: Size of image
  long long int w_, h_;

  //: Number of disparities to search over.
  long long int num_disparities_;

  //: All appearance and smoothing costs will be normalized to discrete
  // values such that this unit corresponds to 1.0 standard deviation of
  // expected appearance error.
  unsigned char cost_unit_;

  //: Hard-coded internal P1, P2 costs, as a scale factor of the cost_unit
  float p1_base_, p2_min_base_, p2_max_base_;

  //: Raw storage for the cost volumes
  // Element x,y,d is at location y*w_*num_disparities + x*num_disparities + d
  std::vector<unsigned short> total_cost_data_;
  std::vector<unsigned char> fused_cost_data_;

  //: Convenience image of pointers into the cost volumes
  std::vector< std::vector< unsigned short* > > total_cost_;
  std::vector< std::vector< unsigned char* > > fused_cost_;

  std::vector< std::vector< unsigned char* > >* active_app_cost_;

  //
  // Sub-routines called by SGM in order
  //

  //: Allocate and setup cost volumes based on current w_ and h_
  void setup_cost_volume(
    std::vector<unsigned char>& cost_data,
    std::vector< std::vector< unsigned char* > >& cost,
    long long int depth );
  void setup_cost_volume(
    std::vector<unsigned short>& cost_data,
    std::vector< std::vector< unsigned short* > >& cost,
    long long int depth );

  //: Compute appearance data costs
  template <class T>
  void compute_census_data(
    const vil_image_view<T>& img_target,
    const vil_image_view<T>& img_ref,
    const vil_image_view<bool>& invalid_target,
    std::vector< std::vector< unsigned char* > >& app_cost,
    const vil_image_view<int>& min_disparity );

  void compute_xgrad_data(
    const vil_image_view<float>& grad_x_target,
    const vil_image_view<float>& grad_x_ref,
    const vil_image_view<bool>& invalid_target,
    std::vector< std::vector< unsigned char* > >& app_cost,
    const vil_image_view<int>& min_disparity );

  //: Run the multi-directional dynamic programming that SGM uses
  void run_multi_dp(
    const std::vector< std::vector<unsigned char*> >& app_cost,
    std::vector< std::vector<unsigned short*> >& total_cost,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<float>& grad_x,
    const vil_image_view<float>& grad_y,
    const vil_image_view<int>& min_disparity );

  //: Pixel-wise directional cost
  inline void compute_dir_cost(
    const unsigned short* prev_row_cost,
    const unsigned char* cur_app_cost,
    unsigned short* cur_row_cost,
    unsigned short* total_cost,
    unsigned short p1,
    unsigned short p2,
    int prev_min_disparity,
    int cur_min_disparity );

  //: Extract the min cost disparity at each pixel, using quadratic
  // interpolation if specified
  void compute_best_disparity_img(
    const std::vector< std::vector< unsigned short* > >& total_cost,
    const vil_image_view<int>& min_disparity,
    const vil_image_view<bool>& invalid_target,
    float invalid_disparity,
    vil_image_view<float>& disp_img,
    vil_image_view<unsigned short>& disp_cost );


  //
  // Other
  //

  //: Convenience function for printing time since last call to this function
  void print_time(
    const char* name,
    vul_timer& timer );

  //: Disable default constructor
  bsgm_disparity_estimator()= default;
};

//-----------------------------------------------------------------------
template <class T>
void bsgm_disparity_estimator::compute_census_data(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
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
      
      unsigned char* ac = app_cost[y][x];

      // If invalid pixel, fill with 255
      if( invalid_tar(x,y) ){
        for( int d = 0; d < num_disparities_; d++, ac++ )
          *ac = 255;
        continue;
      }

      // Target census values
      vxl_uint_64 cen_t = census_tar(x,y);
      vxl_uint_64 conf_t = census_conf_tar(x,y);

      // Compute all costs
      int x2 = x + min_disparity(x,y);
      for ( int d = 0; d < num_disparities_; d++, x2++, ac++ ) {

        // Check valid match pixel
        if( x2 < 0 || x2 >= w_ )
          *ac = 255;

        // Compare census values using hamming distance
        else {

          // reference census values
          vxl_uint_64 cen_r = census_ref(x2,y);
          vxl_uint_64 conf_r = census_conf_ref(x2,y);

          // census comparison
          unsigned long long int cen_diff =
            bsgm_compute_diff_string( cen_t, cen_r, conf_t, conf_r );

          unsigned char ham =
            bsgm_compute_hamming_lut( cen_diff, bit_set_table, only_32_bits );

          float ham_norm = census_norm*ham;
          // weighted update of appearance cost
          float ac_new = (float)(*ac) + params_.census_weight*ham_norm;
          
          *ac = (unsigned char)( ac_new > 255.0f ? 255.0f : ac_new );
        }

      } //d
    } //j
  } //i
}

//----------------------------------------------------------------------------
template <class T>
bool bsgm_disparity_estimator::compute(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  const vil_image_view<bool>& invalid_tar,
  const vil_image_view<int>& min_disp,
  float invalid_disp,
  vil_image_view<float>& disp_tar,
  float dynamic_range_factor,
  bool skip_error_check)
{
  disp_tar.set_size( w_, h_ );

  // Validate images.
  if( img_tar.ni() != w_ || img_tar.nj() != h_ ||
      img_ref.ni() != w_ || img_ref.nj() != h_ ||
      invalid_tar.ni() != w_ || invalid_tar.nj() != h_ ) return false;

  
  long long int num_voxels = w_*h_*num_disparities_;
  // determine appearance scale factor
  float gscale = 1.0f; //SW18 has gscale = 0.32f
  T app_scale = std::numeric_limits<T>::max();
  if( app_scale> T(255)){
    params_.census_tol *=dynamic_range_factor;//SW18 has params_.census_tol *=20
    gscale = 1.0f/dynamic_range_factor;
  }
  vul_timer timer, total_timer;
  if( params_.print_timing ){
    timer.mark(); total_timer.mark();
  }

  // Compute gradient images.
  vil_image_view<float> grad_x_tar, grad_y_tar, grad_x_ref, grad_y_ref;
  if (params_.use_gradient_weighted_smoothing ||
    params_.xgrad_weight > 0.0f) {
    vil_sobel_3x3<T, float>(img_tar, grad_x_tar, grad_y_tar);

    if (app_scale > T(255)) {
      vil_math_scale_values(grad_x_tar, gscale);
      vil_math_scale_values(grad_y_tar, gscale);
    }
  }
  if( params_.print_timing )
    print_time( "Gradient image computation", timer );

  // Compute appearance cost volume data.
  if( params_.census_weight > 0.0f ){
    compute_census_data<T>( img_tar, img_ref,
      invalid_tar, fused_cost_, min_disp );
  }

  if( params_.xgrad_weight > 0.0f ){
    vil_sobel_3x3<T,float>( img_ref, grad_x_ref, grad_y_ref );

    if (app_scale > T(255)) {
      vil_math_scale_values(grad_x_ref, gscale);
      vil_math_scale_values(grad_y_ref, gscale);
    }
    compute_xgrad_data( grad_x_tar, grad_x_ref,
      invalid_tar, fused_cost_, min_disp );
  }

  active_app_cost_ = &fused_cost_;

  if( params_.print_timing )
    print_time( "Appearance cost computation", timer );

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
  if( params_.error_check_mode > 0 && !skip_error_check){
    bsgm_check_nonunique<T>( disp_tar, disp_cost,
      img_tar, invalid_disp, params_.shadow_thresh);

    if( params_.error_check_mode > 1 )
      bsgm_interpolate_errors<T>( disp_tar, invalid_tar,
        img_tar, invalid_disp, params_.shadow_thresh);

    if (params_.print_timing)
      print_time("Consistency check", timer);
  }

  if( params_.print_timing )
    print_time( "TOTAL TIME", total_timer );

  return true;
}



#endif // bsgm_disparity_estimator_h_
