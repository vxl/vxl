// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef bsgm_disparity_estimator_h_
#define bsgm_disparity_estimator_h_

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <utility>

#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>

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
  vxl_byte shadow_thresh;

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
    census_weight(0.3f),
    xgrad_weight(0.7f),
    census_tol(2),
    census_rad(2),
    print_timing(false){}

};


class bsgm_disparity_estimator
{
 public:

  //: Construct from parameters
  bsgm_disparity_estimator(
    const bsgm_disparity_estimator_params& params,
    int img_width,
    int img_height,
    int num_disparities );

  //: Destructor
  ~bsgm_disparity_estimator();

  //: Run the SGM algorithm to compute a disparity map for the img_target,
  // where disparities measure the displacement of the reference pixel from
  // the target pixel i.e.
  //   img_target(x,y) <-> img_ref( x + disp_target(x,y), y )
  // Note that this is the opposite of the OpenCV SGM implementation.
  bool compute(
    const vil_image_view<vxl_byte>& img_target,
    const vil_image_view<vxl_byte>& img_ref,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<int>& min_disparity,
    float invalid_disparity,
    vil_image_view<float>& disp_target );

  //: Write out the appearance or total cost volume as a set of images for
  // debugging
  void write_cost_debug_imgs(
    const std::string& out_dir,
    bool write_total_cost = false );

 protected:

  //: Copy of the input params
  bsgm_disparity_estimator_params params_;

  //: Size of image
  int w_, h_;

  //: Number of disparities to search over.
  int num_disparities_;

  //: All appearance and smoothing costs will be normalized to discrete
  // values such that this unit corresponds to 1.0 standard deviation of
  // expected appearance error.
  unsigned char cost_unit_;

  //: Hard-coded internal P1, P2 costs, as a scale factor of the cost_unit
  float p1_base_, p2_min_base_, p2_max_base_;

  //: Raw storage for the cost volumes
  // Element x,y,d is at location y*w_*num_disparities + x*num_disparities + d
  std::vector<unsigned short> total_cost_data_;
  std::vector<unsigned char> census_cost_data_;
  std::vector<unsigned char> xgrad_cost_data_;
  std::vector<unsigned char> fused_cost_data_;

  //: Convenience image of pointers into the cost volumes
  std::vector< std::vector< unsigned short* > > total_cost_;
  std::vector< std::vector< unsigned char* > > census_cost_;
  std::vector< std::vector< unsigned char* > > xgrad_cost_;
  std::vector< std::vector< unsigned char* > > fused_cost_;

  std::vector< std::vector< unsigned char* > >* active_app_cost_;


  //
  // Sub-routines called by SGM in order
  //

  //: Allocate and setup cost volumes based on current w_ and h_
  void setup_cost_volume(
    std::vector<unsigned char>& cost_data,
    std::vector< std::vector< unsigned char* > >& cost,
    int depth );
  void setup_cost_volume(
    std::vector<unsigned short>& cost_data,
    std::vector< std::vector< unsigned short* > >& cost,
    int depth );

  //: Compute appearance data costs
  void compute_census_data(
    const vil_image_view<vxl_byte>& img_target,
    const vil_image_view<vxl_byte>& img_ref,
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

  //: Use the OpenCV SGM uniqueness criteria to find bad disparities. This
  // is not quite the same as the left-right consistency check from the SGM
  // paper.  Despite working well enough, this approach is problematic in part
  // because the overflow-normalized disparity costs of different pixels
  // should not be directly compared, but they are.
  void flag_nonunique(
    vil_image_view<float>& disp_img,
    const vil_image_view<unsigned short>& disp_cost,
    const vil_image_view<vxl_byte>& img,
    float invalid_disparity,
    int disp_thresh = 1 );

  //: Fill in disparity pixels flagged as errors via multi-directional
  // sampling.
  void interpolate_errors(
    vil_image_view<float>& disp_img,
    const vil_image_view<bool>& invalid,
    const vil_image_view<vxl_byte>& img,
    float invalid_disparity );


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


// TO BE MOVED

//: Compute a map of invalid pixels based on seeing the 'border_val'
// in either target or reference images.
void compute_invalid_map(
  const vil_image_view<vxl_byte>& img_target,
  const vil_image_view<vxl_byte>& img_ref,
  vil_image_view<bool>& invalid_target,
  int min_disparity,
  int num_disparities,
  vxl_byte border_val = (unsigned char)0 );

//: Flip the sign of all disparities, swap invalid values.
void bsgm_invert_disparities(
  vil_image_view<float>& disp_img,
  int old_invalid,
  int new_invalid );


#endif // bsgm_disparity_estimator_h_
