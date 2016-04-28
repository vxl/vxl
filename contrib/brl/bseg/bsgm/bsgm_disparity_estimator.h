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
  //: The minimum disparity to consider, can be negative.
  int min_disparity = 0;

  //: The number of disparities to search over, range will be
  // [ min_disparity, min_disparity + num_disparities )
  int num_disparities = 20;

  //: Use 16 directions in the dynamic programming, otherwise 8.  This 
  // roughly doubles computation time.
  bool use_16_directions = false;

  //: Scale the internally set P1, P2 smoothing parameters.
  float p1_scale = 1.0f;
  float p2_scale = 1.0f;

  //: Use gradient-weighted P2 smoothing, as suggested in paper.
  bool use_gradient_weighted_smoothing = true;

  //: In gradient-weighted smoothing, gradients beyond this magnitude are
  // truncated.
  float max_grad = 32.0f;

  //: Use quadratic interpolation to obtain sub-pixel estimates of final
  // disparity map.
  bool perform_quadratic_interp = true;

  //: Perform a quick uniqueness check to flag invalid disparities.
  bool perform_consistency_check = true;

  //: Identify rectification border pixels by this intensity and remove
  // from computation.  Set < 0 to disable.
  int border_val = 0;

  //: Appearance costs computed by different algorithms are statically fused 
  // using these weights. Set any to <= 0 to prevent computation.
  float census_weight = 0.3f;
  float xgrad_weight = 0.7f;

  //: Pixel differences less than this magnitude are not considered in the
  // census computation.  Increase to prevent errors from sensor noise. 
  // Set to 0 for textbook census implementation.
  int census_tol = 2;

  //: Set this to treat all input/output disparities as the displacement
  // of the target pixel with respect to the ref pixel, as the OpenCV 
  // implementation does.
  bool using_ref_to_target_disparities = true;

  //: Print detailed timing information to cerr.
  bool print_timing_ = false;

};


class bsgm_disparity_estimator
{
 public:

  //: Construct from parameters
  bsgm_disparity_estimator(
    const bsgm_disparity_estimator_params& params );

  //: Destructor
  ~bsgm_disparity_estimator();

  //: Run the SGM algorithm
  bool compute(
    const vil_image_view<vxl_byte>& img_target,
    const vil_image_view<vxl_byte>& img_ref,
    vil_image_view<float>& disp_target );

  //: Write out the appearance or total cost volume as a set of images for 
  // debugging
  void write_cost_debug_imgs(
    const std::string& out_dir,
    bool write_total_cost = false );

 protected:

  //: Copy of the input params
  bsgm_disparity_estimator_params params_;

  //: Size of last image
  int w_, h_;

  //: All appearance and smoothing costs will be represented as discrete 
  // numbers such that this unit corresponds to 1.0 standard deviation of 
  // expected appearance error.
  unsigned char cost_unit_;

  //: Hard-coded internal P1, P2 costs, as a scale factor of the cost_unit
  float p1_base_, p2_min_base_, p2_max_base_;

  //: This value will indicate invalid disparities.
  float invalid_disp_;

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

  //: Compute a map of invalid pixels based on seeing the 'invalid_val'
  // in either target or reference images.
  void compute_invalid_map(
    const vil_image_view<vxl_byte>& img_target,
    const vil_image_view<vxl_byte>& img_ref,
    vil_image_view<bool>& invalid_target );

  //: Allocate and setup cost volumes based on current w_ and h_
  void setup_cost_volume( 
    std::vector<unsigned char>& cost_data,
    std::vector< std::vector< unsigned char* > >& cost );
  void setup_cost_volume( 
    std::vector<unsigned short>& cost_data,
    std::vector< std::vector< unsigned short* > >& cost );

  //: Compute appearance data costs
  void compute_census_data(
    const vil_image_view<vxl_byte>& img_target,
    const vil_image_view<vxl_byte>& img_ref,
    const vil_image_view<bool>& invalid_target,
    std::vector< std::vector< unsigned char* > >& app_cost );
  void compute_xgrad_data(
    const vil_image_view<float>& grad_x_target,
    const vil_image_view<float>& grad_x_ref,
    const vil_image_view<bool>& invalid_target,
    std::vector< std::vector< unsigned char* > >& app_cost );

  //: Run the multi-directional dynamic programming that SGM uses
  void run_multi_dp(
    const std::vector< std::vector<unsigned char*> >& app_cost,
    std::vector< std::vector<unsigned short*> >& total_cost,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<float>& grad_x,
    const vil_image_view<float>& grad_y );

  //: Pixel-wise directional cost
  void compute_dir_cost(
    const unsigned short* prev_row_cost,
    const unsigned char* cur_app_cost,
    unsigned short* cur_row_cost,
    unsigned short p1,
    unsigned short p2 );

  //: Extract the min cost disparity at each pixel, using quadratic
  // interpolation if specified
  void compute_best_disparity_img(
    const std::vector< std::vector< unsigned short* > >& total_cost,
    const vil_image_view<bool>& invalid_target,
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
    int disp_thresh = 1 );

  //: Flip the sign of all valid disparities.
  void invert_disparities(
    vil_image_view<float>& disp_img );


  //
  // Other
  // 

  //: Convenience function for printing time since last call to this function
  void print_time(
    const char* name, 
    vul_timer& timer );

  //: Disable default constructor
  bsgm_disparity_estimator(){}
};



#endif // bsgm_disparity_estimator_h_
