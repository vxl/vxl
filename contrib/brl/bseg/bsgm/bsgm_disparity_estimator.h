// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef bsgm_disparity_estimator_h_
#define bsgm_disparity_estimator_h_

#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <mutex>

#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vil/vil_crop.h>
#include <vil/vil_copy.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vgl/vgl_box_2d.h>
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

  //: Adjust the value of P2 based on the probability of shadow step response
  bool use_shadow_step_p2_adjustment;

  //: Use quadratic interpolation to obtain sub-pixel estimates of final
  // disparity map.
  bool perform_quadratic_interp;

  //: Mode for finding and fixing errors in the disparity map
  // 0 raw disparity map
  // 1 bad pixels flagged
  // 2 bad pixels interpolated over
  int error_check_mode;

  //: When set > 0, pixels below this threshold will be flagged as invalid
  // applied for any error_check_mode
  unsigned short shadow_thresh;

  //: Set "bias_weight" to the range (0.0,1.0] to bias the SGM directional average
  // against the sun_dir_tar_ vector.  Use this if smoothing from certain
  // directions (i.e. sun angle for satellite imagery) is unreliable.  Set to
  // 0 to disable biasing. (deprecated, supplanted by adj_dir_weight)
  float bias_weight;
  
  //: Under shadow step and shadow control of the dynamic program
  //  scan with prior cost empthasized in the scan direction opposite to the sun rays,
  //  also include directions to each side of this primary direction with weight
  //  defined by "adj_dir_weight" in the interval (0, 1) 
  float adj_dir_weight;

  //: suppress appearance cost in dynamic program for both shadow and shadow step
  // pixels otherwise only suppress in shadow
  bool app_supress_shadow_shad_step;

  //: threhold for considering either shadow or shadow step active
  float shad_shad_stp_prob_thresh;

  //: Appearance costs computed by different algorithms are statically fused
  // using these weights. Set any to <= 0 to prevent computation.
  float census_weight;
  float xgrad_weight;

  //: Pixel differences less than this magnitude are not considered in the
  // census computation.  Increase to prevent errors from sensor noise.
  // Set to 0 for textbook census implementation.
  int census_tol;

  //: The length of the census kernel will be 2*census_rad+1. Must be 1,2,or 3.
  int census_rad;

  //: Print detailed timing information to cerr.
  bool print_timing;

  //: Default parameters
  bsgm_disparity_estimator_params():
    use_16_directions(false),
    p1_scale(1.0f),
    p2_scale(1.0f),
    use_gradient_weighted_smoothing(true),
    use_shadow_step_p2_adjustment(false),
    max_grad(32.0f),
    perform_quadratic_interp(true),
    error_check_mode(1),
    shadow_thresh(0),
    bias_weight(0.0f),
    adj_dir_weight(0.25f),
    //adj_dir_weight(0.0f),
    app_supress_shadow_shad_step(false),
    shad_shad_stp_prob_thresh(0.5f),
    census_weight(0.3f),
    xgrad_weight(0.7f),
    census_tol(2),
    census_rad(2),
    print_timing(false)
    {}
};

// output parameters
std::ostream& operator<<(std::ostream& os, const bsgm_disparity_estimator_params& params);


class bsgm_disparity_estimator
{
 public:

  //: Construct from parameters
  bsgm_disparity_estimator(
    const bsgm_disparity_estimator_params& params,
    long long int cost_volume_width,
    long long int cost_volume_height,
    long long int num_disparities,
    // optional shadow processing to eliminate overhang
    // must be defined if use_shadow_step_p2_adjustment = true,
    vil_image_view<float> const& shadow_step_prob = vil_image_view<float>(),
    vil_image_view<float> const& shadow_prob = vil_image_view<float>(),
    vgl_vector_2d<float> const& sun_dir_tar = vgl_vector_2d<float>(0.0f, 0.0f));
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
    bool skip_error_check = false,
    const vgl_box_2d<int>& target_window = vgl_box_2d<int>(),
    vgl_box_2d<int> reference_window = vgl_box_2d<int>());

  //: Write out the appearance or total cost volume as a set of images for
  // debugging
  void write_cost_debug_imgs(
    const std::string& out_dir,
    bool write_total_cost = false );

 protected:

  //: Copy of the input params
  bsgm_disparity_estimator_params params_;

  //: Size of cost volume
  long long int w_, h_;

  //: Number of disparities to search over.
  unsigned long long int num_disparities_;

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

  //: shadow step probability in rectified image space
  vil_image_view<float> shadow_step_prob_;

  //: shadow mask
  vil_image_view<float> shadow_prob_;

  //: sun direction in rectified image space
  vgl_vector_2d<float> sun_dir_tar_;

  vil_image_view<unsigned short> img_tar_;

  //
  // Sub-routines called by SGM in order
  //
  // compute shadow info, in case not supplied externally
  template <class T>
  void compute_shadow_prob(const vil_image_view<T>& img_target, vil_image_view<bool> const& invalid_target){
    size_t w = img_target.ni(), h = img_target.nj();
    shadow_prob_.set_size(w, h);
    shadow_prob_.fill(0.0f);
    for(size_t y = 0; y<h; ++y)
      for(size_t x = 0; x<w; ++x)
        if(img_target(x, y)<params_.shadow_thresh)
          shadow_prob_(x,y) = 1.0f;
  }
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
    const vil_image_view<int>& min_disparity,
    const vgl_box_2d<int>& target_window = vgl_box_2d<int>(),
    const vgl_box_2d<int>& reference_window = vgl_box_2d<int>());

  void compute_xgrad_data(
    const vil_image_view<float>& grad_x_target,
    const vil_image_view<float>& grad_x_ref,
    const vil_image_view<bool>& invalid_target,
    std::vector< std::vector< unsigned char* > >& app_cost,
    const vil_image_view<int>& min_disparity,
    const vgl_box_2d<int>& target_window = vgl_box_2d<int>());

  void compute_shadow_data(
    const vil_image_view<bool>& invalid_target,
    //std::vector< std::vector< unsigned char* > >& app_cost,
    std::vector< std::vector< unsigned short* > >& app_cost,
    const vil_image_view<int>& min_disparity,
    const vgl_box_2d<int>& target_window = vgl_box_2d<int>());

  //: Run the multi-directional dynamic programming that SGM uses
  void run_multi_dp(
    const std::vector< std::vector<unsigned char*> >& app_cost,
    std::vector< std::vector<unsigned short*> >& total_cost,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<float>& grad_x,
    const vil_image_view<float>& grad_y,
    const vil_image_view<int>& min_disparity);

  void run_multi_dp_opt(
    const std::vector< std::vector<unsigned char*> >& app_cost,
    std::vector< std::vector<unsigned short*> >& total_cost,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<float>& grad_x,
    const vil_image_view<float>& grad_y,
    const vil_image_view<int>& min_disparity
  );

  // Necessary arguments utilized by all specialized directional DP functions
  struct dp_args {
    const float bias_weight;
    const float mag;
    const vgl_vector_2d<float>& bias_dir;
    const bool using_bias;
    const bool shad_step_dynamic_prog;
    const std::vector<vil_image_view<float>>& deriv_img;
    const unsigned short p1;
    const float p2_min;
    const float p2_max;
    const unsigned short p2;
    const int shad_step_dp_dir_code;
    const std::vector<std::pair<int, int>>& adj_dirs;

    // These are dependent on direction of DP sweep
    int dir;
    int deriv_idx;
  };

  std::vector<std::mutex> cost_mutexes;

  //: Run the dynamic programming that SGM uses in horizontal sweep
  void run_horizontal_dp(
    const std::vector< std::vector<unsigned char*> >& app_cost,
    std::vector< std::vector<unsigned short*> >& total_cost,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<int>& min_disparity,
    bool move_right,
    const dp_args& args
  );
  
  //: Run the dynamic programming that SGM uses in vertical sweep
  void run_vertical_dp(
    const std::vector< std::vector<unsigned char*> >& app_cost,
    std::vector< std::vector<unsigned short*> >& total_cost,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<int>& min_disparity,
    bool move_down,
    const dp_args& args
  );

  //: Run the dynamic programming that SGM uses in diagonal sweep
  void run_diag_dp(
    const std::vector< std::vector<unsigned char*> >& app_cost,
    std::vector< std::vector<unsigned short*> >& total_cost,
    const vil_image_view<bool>& invalid_target,
    const vil_image_view<int>& min_disparity,
    bool move_right,
    bool move_down,
    const dp_args& args
  );

  // Calculate directional weight if necessary
  // deprecated method to reduce shadow overhang
  inline float calc_dir_weight(float dx, float dy, const dp_args& args) {  
    float dir_weight = 1.0f;
    if (args.using_bias) {
      vgl_vector_2d<float> dp_dir(dx, dy);
      dp_dir = normalize(dp_dir);
      dp_dir *= -1.0f; //low interpolation weight in shadow direction
      float cosa = dot_product(dp_dir, args.bias_dir);
      dir_weight = 1.0f - args.bias_weight * 0.5f * (1.0f - cosa);
      //std::cout << "in dynamic program: (dx dy) ("<< dx << ' ' << dy << ") dp_dir "<< dp_dir << " bias dir " << bias_dir << " cosa = " << cosa << " dir weight = " << dir_weight << std::endl;
    }
    return dir_weight;
  }

  // Processes an (x, y) position during a directional sweep for SGM.
  // Returns the disparity with the minimum cost value for that position
  inline long long process_pos(
    long long x, long long y,
    int dx, int dy,
    float dir_weight, 
    const std::vector<unsigned short>& prev_pos_cost,
    std::vector<unsigned short>& cur_pos_cost,
    long long prev_min_d,
    const vil_image_view<int>& min_disparity,
    const dp_args& args
  );

  //: Pixel-wise directional cost
  inline void compute_dir_cost(
    const unsigned short* prev_row_cost,
    const unsigned char* cur_app_cost,
    unsigned short* cur_row_cost,
    unsigned short* total_cost,
    unsigned short p1,
    unsigned short p2,
    int prev_min_disparity,
    int cur_min_disparity,
    bool suppress_appearance = false,
    float adj_weight = 0.0f
  );

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

  template <class srcT, class destT>
  void gradient_inside_window(vil_image_view<srcT> const& img,
                              vgl_box_2d<int> const& window,
                              vil_image_view<destT>& grad_i,
                              vil_image_view<destT>& grad_j);

  vgl_box_2d<int> add_margin_to_window(
    const vgl_box_2d<int>& target_window,
    int margin,
    int ni,
    int nj);

  //: Convenience function for printing time since last call to this function
  void print_time(
    const char* name,
    vul_timer& timer );

  //: Disable default constructor
  bsgm_disparity_estimator()= default;
};

//-----------------------------------------------------------------------
template <class srcT, class destT>
void bsgm_disparity_estimator::gradient_inside_window(
    vil_image_view<srcT> const& img,
    vgl_box_2d<int> const& window,
    vil_image_view<destT>& grad_i,
    vil_image_view<destT>& grad_j)
{
  // TODO: Avoid unnecessary copies, make this more efficient

  /* vil_image_resource_sptr img_sptr = vil_new_image_resource_of_view(img); */
  /* vil_image_resource_sptr img_cropped_sptr = vil_crop( */
  /*     img_sptr, */
  /*     window.min_x(), window.width(), */
  /*     window.min_y(), window.height()); */

  // crop image data inside window
  vil_image_view<srcT> img_cropped = vil_crop(
      img,
      window.min_x(), window.width(),
      window.min_y(), window.height());

  // get gradient data within window
  vil_image_view<destT> grad_i_cropped, grad_j_cropped;
  vil_sobel_3x3<srcT, destT>(img_cropped, grad_i_cropped, grad_j_cropped);

  // put that data into full-sized empty images, uninitialized outside of the window
  int ni = img.ni();
  int nj = img.nj();
  int np = img.nplanes();
  grad_i.set_size(ni, nj, np);
  grad_j.set_size(ni, nj, np);

  vil_copy_to_window(grad_i_cropped, grad_i,
                      window.min_x(), window.min_y());
  vil_copy_to_window(grad_j_cropped, grad_j,
                      window.min_x(), window.min_y());
}

//-----------------------------------------------------------------------
template <class T>
void bsgm_disparity_estimator::compute_census_data(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  const vil_image_view<bool>& invalid_tar,
  std::vector< std::vector< unsigned char* > >& app_cost,
  const vil_image_view<int>& min_disparity,
  const vgl_box_2d<int>& target_window,
  const vgl_box_2d<int>& reference_window)
{
  vul_timer t;

  // target and reference images have same size
  int ni = static_cast<int>(img_tar.ni()), nj = static_cast<int>(img_tar.nj());

  vgl_box_2d<int> tar_window_with_margin(0, ni, 0, nj);
  vgl_box_2d<int> ref_window_with_margin(0, ni, 0, nj);
  int img_start_x, img_start_y;
  if (target_window.is_empty()) {
    img_start_x = 0;
    img_start_y = 0;
  }
  else {
    img_start_x = target_window.min_x();
    img_start_y = target_window.min_y();

    // census computes each pixel using the surrounding neighborhood, so need a margin
    tar_window_with_margin = add_margin_to_window(target_window, params_.census_rad, ni, nj);
    ref_window_with_margin = add_margin_to_window(reference_window, params_.census_rad, ni, nj);
  }

  int census_diam = 2*params_.census_rad + 1;
  if( census_diam > 7 ) census_diam = 7;
  if( census_diam < 3 ) census_diam = 3;
  float census_norm = 8.0f*cost_unit_/(float)(census_diam*census_diam);
  bool only_32_bits = census_diam <= 5;

  // Compute census images
  vil_image_view<vxl_uint_64> census_tar, census_ref;
  vil_image_view<vxl_uint_64> census_conf_tar, census_conf_ref;
  bsgm_compute_census_img(
    img_tar, census_diam, census_tar, census_conf_tar, params_.census_tol, tar_window_with_margin);
  bsgm_compute_census_img(
    img_ref, census_diam, census_ref, census_conf_ref, params_.census_tol, ref_window_with_margin);

  // timer report
  if (params_.print_timing)
    print_time("Census images", t);
  /* std::cout << "compute census images " << t.real() << " msec." << std::endl; */
  /* t.mark(); */

  // Construct a bit-set look-up table for use later
  unsigned char bit_set_table[256];
  bsgm_generate_bit_set_lut( bit_set_table );

  // Compute the appearance cost volume
  // (keep track of SGM cost volume indices, and the corresponding target image indices)
  for (int cost_y = 0, img_y = img_start_y; cost_y < h_; cost_y++, img_y++) {
    for (int cost_x = 0, img_x = img_start_x; cost_x < w_; cost_x++, img_x++) {

      unsigned char* ac = app_cost[cost_y][cost_x];

      // If invalid pixel, fill with 255
      if (invalid_tar(cost_x, cost_y)) {
        for( int d = 0; d < num_disparities_; d++, ac++ )
          *ac = 255;
        continue;
      }

      // Target census values
      vxl_uint_64 cen_t = census_tar(img_x, img_y);
      vxl_uint_64 conf_t = census_conf_tar(img_x, img_y);

      // Compute all costs
      int img_x2 = img_x + min_disparity(cost_x, cost_y);
      for (int d = 0; d < num_disparities_; d++, img_x2++, ac++) {

        // Check valid match pixel
        /* if (img_x2 < 0 || img_x2 >= w_) */
        if (img_x2 < 0 || img_x2 >= ni)
          *ac = 255;

        // Compare census values using hamming distance
        else {

          // reference census values
          vxl_uint_64 cen_r = census_ref(img_x2, img_y);
          vxl_uint_64 conf_r = census_conf_ref(img_x2, img_y);

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
  if (params_.print_timing)
    print_time("Census appearance cost", t);
  /* std::cout << "" << t.real() << " msec." << std::endl; */
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
  bool skip_error_check,
  const vgl_box_2d<int>& target_window,
  vgl_box_2d<int> reference_window)
{
  // validate target image is big enough for the cost volume
  if (target_window.is_empty()) {
    if (img_tar.ni() != w_ || img_tar.nj() != h_){
      throw std::runtime_error("target image not the same size as cost volume");
    }
  }
  else {
    // sgm cost volume should have same size as target window
    if (target_window.width() != w_ || target_window.height() != h_){
      throw std::runtime_error("target window not the same size as cost volume");
    }
    // target image must be large enough to be indexable by the target window
    if (target_window.min_x() < 0 || img_tar.ni() <= (unsigned)target_window.max_x() ||
        target_window.min_y() < 0 || img_tar.nj() <= (unsigned)target_window.max_y()) {
      throw std::runtime_error("target window outside target image extents");
    }

    // default reference window (full image width)
    if (reference_window.is_empty()) {
      reference_window = vgl_box_2d<int>(0, img_tar.ni(), target_window.min_y(), target_window.max_y());
    }

    // reference image must be large enough to be indexable by the reference window
    if (reference_window.min_x() < 0 || img_ref.ni() <= (unsigned)reference_window.max_x() ||
        reference_window.min_y() < 0 || img_ref.nj() <= (unsigned)reference_window.max_y()) {
      throw std::runtime_error("reference window outside reference image extents");
    }
  }

  // validate that the reference image is the same size as the target image
  if (img_ref.ni() != img_tar.ni() || img_ref.nj() != img_tar.nj()){
    throw std::runtime_error("target image and reference image have different shapes");
  }
  // these two images should always have the same size as the cost volume
  if (invalid_tar.ni() != w_ || invalid_tar.nj() != h_){
    throw std::runtime_error("invalid image different shape than cost volume");
  }
  if (min_disp.ni() != w_ || min_disp.nj() != h_){
    throw std::runtime_error("minimum disparity different shape than cost volume");
  }
  img_tar_ = img_tar;

  // disparity image
  disp_tar.set_size(w_, h_);

  // target and reference images have same size
  int ni = static_cast<int>(img_tar.ni()), nj = static_cast<int>(img_tar.nj());
  long long int num_voxels = w_ * h_ * num_disparities_;

  // determine appearance scale factor
  float gscale = 1.0f; //SW18 has gscale = 0.32f
  T app_scale = std::numeric_limits<T>::max();
  if( app_scale> T(255)){
    params_.census_tol *=dynamic_range_factor;//SW18 has params_.census_tol *=20
    gscale = 1.0f/dynamic_range_factor;
  }
  // shadow info - (if commented out, input from prob_pairwise_dsm instead of local computation)
  //compute_shadow_prob(img_tar, invalid_tar);

  // Compute census appearance cost volume data.
  if (params_.census_weight > 0.0f) {
    compute_census_data<T>(img_tar, img_ref, invalid_tar, fused_cost_,
                           min_disp, target_window, reference_window);
  }

  vul_timer timer, total_timer;
  if (params_.print_timing) {
    timer.mark(); total_timer.mark();
  }

  // Compute gradient appearance cost volume data.
  vil_image_view<float> grad_x_tar, grad_y_tar;
  if (params_.use_gradient_weighted_smoothing || params_.xgrad_weight > 0.0f) {

    if (target_window.is_empty()) {
      vil_sobel_3x3<T, float>(img_tar, grad_x_tar, grad_y_tar);
    }
    else {
      // xgrad computed with 3x3 kernel, so need a margin of 1 pixel
      vgl_box_2d<int> tar_window_with_xgrad_margin = add_margin_to_window(target_window, 1, ni, nj);

      /* // crop target image data inside window */
      /* vil_image_view<float> img_tar_cropped = vil_crop( */
      /*     img_tar, */
      /*     tar_window_with_xgrad_margin.min_x(), */
      /*     tar_window_with_xgrad_margin.width(), */
      /*     tar_window_with_xgrad_margin.min_y(), */
      /*     tar_window_with_xgrad_margin.height()); */

      /* // get gradient data of crop */
      /* vil_sobel_3x3<T, float>(img_tar_cropped, grad_x_tar, grad_y_tar); */

      // get full sized gradient images, uninitialized outside of the target window
      /* vil_sobel_3x3<T, float>(img_tar, tar_window_with_xgrad_margin, grad_x_tar, grad_y_tar); */
      gradient_inside_window(img_tar, tar_window_with_xgrad_margin,
                             grad_x_tar, grad_y_tar);
    }

    if (app_scale > T(255)) {
      vil_math_scale_values(grad_x_tar, gscale);
      vil_math_scale_values(grad_y_tar, gscale);
    }

    if (params_.print_timing)
      print_time("Target gradient image", timer);

    // Reference gradients
    vil_image_view<float> grad_x_ref, grad_y_ref;
    if (target_window.is_empty()) {
      vil_sobel_3x3<T,float>(img_ref, grad_x_ref, grad_y_ref);
    }
    else {
      // xgrad computed with 3x3 kernel, so need a margin of 1 pixel
      vgl_box_2d<int> ref_window_with_xgrad_margin = add_margin_to_window(reference_window, 1, ni, nj);

      /* // crop reference image data inside window */
      /* vil_image_view<float> img_ref_cropped = vil_crop( */
      /*     img_ref, */
      /*     ref_window_with_xgrad_margin.min_x(), */
      /*     ref_window_with_xgrad_margin.width(), */
      /*     ref_window_with_xgrad_margin.min_y(), */
      /*     ref_window_with_xgrad_margin.height()); */

      /* // get gradient data of crop */
      /* vil_sobel_3x3<T, float>(img_ref_cropped, grad_x_ref, grad_y_ref); */

      // get full sized gradient images, uninitialized outside of the target window
      /* vil_sobel_3x3<T,float>(img_ref, ref_window_with_xgrad_margin, grad_x_ref, grad_y_ref); */
      gradient_inside_window(img_ref, ref_window_with_xgrad_margin,
                             grad_x_ref, grad_y_ref);
    }

    if (app_scale > T(255)) {
      vil_math_scale_values(grad_x_ref, gscale);
      vil_math_scale_values(grad_y_ref, gscale);
    }

    if (params_.print_timing)
      print_time("Reference gradient image", timer);

    compute_xgrad_data(grad_x_tar, grad_x_ref, invalid_tar, fused_cost_,
                       min_disp, target_window);
    if (params_.print_timing)
      print_time("XGradient appearance cost", timer);
  }

  active_app_cost_ = &fused_cost_;

  // Crop the target gradient images before passing them to run_multi_dp
  // so that they are the same size as the cost volume.
  vil_image_view<float> grad_x_tar_cropped, grad_y_tar_cropped;
  if (target_window.is_empty()) {
    grad_x_tar_cropped = grad_x_tar;
    grad_y_tar_cropped = grad_y_tar;
  }
  else {
    grad_x_tar_cropped = vil_crop(grad_x_tar, target_window.min_x(),
                                  target_window.width(), target_window.min_y(),
                                  target_window.height());
    grad_y_tar_cropped = vil_crop(grad_y_tar, target_window.min_x(),
                                  target_window.width(), target_window.min_y(),
                                  target_window.height());
  }

  // Run the multi-directional dynamic programming to obtain a total cost
  // volume incorporating appearance + smoothing.
  run_multi_dp_opt(
    *active_app_cost_, total_cost_,
    invalid_tar, grad_x_tar_cropped, grad_y_tar_cropped, min_disp
  );
  // std::vector<unsigned short> simd_cost = total_cost_data_;

  // run_multi_dp(
  //   *active_app_cost_, total_cost_,
  //   invalid_tar, grad_x_tar_cropped, grad_y_tar_cropped, min_disp);
  
  // long long idx = 0;
  // bool found_diff = false;
  // for(long long y = 0; y < h_; y++) {
  //   for(long long x = 0; x < w_; x++) {
  //     for(long long d = 0; d < num_disparities_; d++) {
  //       if(total_cost_data_[idx] != simd_cost[idx]) {
  //         found_diff = true;
  //         std::cout << "Got differing values for cost at (" << x << ", " << y << ", " << d << "): parallelized version gave " << simd_cost[idx] << ", while serial version gave " << total_cost_data_[idx] << std::endl;
  //       }
  //     }
  //   }
  // }
  // if(!found_diff)
  //   std::cout << "Found no differences between both implementations' output!" << std::endl;

  if (params_.print_timing)
    print_time("Dynamic programming", timer);

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

  if (params_.print_timing)
    print_time("Disparity map extraction", timer);

  // Find and fix errors if configured.
  if (!skip_error_check) {

    if(sun_dir_tar_.length()==0.0){
      bsgm_check_shadows<T>(disp_tar, img_tar, invalid_disp,
                            params_.shadow_thresh, target_window);
    }
      
    
    if ( params_.error_check_mode > 0) {
      bsgm_check_nonunique<T>( disp_tar, disp_cost,
        img_tar, invalid_disp, params_.shadow_thresh, 1, target_window);
    }

    if ( params_.error_check_mode > 1 ) {
      bsgm_interpolate_errors<T>( disp_tar, invalid_tar,
        img_tar, invalid_disp, params_.shadow_thresh, target_window);
    }

    if (params_.print_timing)
      print_time("Consistency check", timer);
  }

  if (params_.print_timing)
    print_time("TOTAL COMPUTE TIME", total_timer);

  return true;
}



#endif // bsgm_disparity_estimator_h_
