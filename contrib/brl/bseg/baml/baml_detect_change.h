// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef baml_detect_change_h_
#define baml_detect_change_h_

#include <string>
#include <utility>
#include <vector>
#include <set>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vil/vil_image_view.h>

#include <vul/vul_file.h>
//:
// \file
// \brief
// \author Thomas Pollard & Selene Chew
// \date January 10, 2017


//: Assorted pairwise image change detection algorithms.
enum baml_change_detection_method {
  BIRCHFIELD_TOMASI,
  CENSUS,
  DIFFERENCE,
  GRADIENT_DIFF,
  NON_PARAMETRIC,
  HIST_CMP
};


//: A collection of parameters used by the baml_change_detection class.
struct baml_change_detection_params {

  //: The change detection method used.
  baml_change_detection_method method;

  //: Estimate an optimal gain/offset to radiometrically align the reference
  // image to the target for methods that are intensity dependent. Specify
  // number of tiles wanted for tiled regression (setting num_tiles to 0 or 1
  // yields the non-tiled result
  bool correct_gain_offset;
  int num_tiles;

  //: If > 0, do a brute-force translational search in x and y with this radius
  // to find the alignment which minimizes average change probability in the
  // region.
  int registration_refinement_rad;

  //: Prior on change probability, will determine the scale of the posterior
  // change distribution.
  float pChange;

  //: Event trigger parameters
  int event_width;
  int event_height;

  // Birchfield-Tomasi specific parameters
 // float bt_std;
  int bt_rad;

  // Census parameters
  // census_tol: pixel differences less than this magnitude are not considered
  //   in the census computation.  Increase to prevent errors from sensor noise.
  //   Set to 0 for textbook census implementation.
  // census_rad: length of the census kernal will be 2*census_rad+1. Must be
  //    1,2,or 3.
  int census_tol;
  int census_rad;


  // Non-parametric parameters
  int img_bit_depth;
  int hist_bit_depth;

  // Histogram Comparison parameters
  // neighborhood_size: score will be computed by using histogram commparison technique
  //    for histograms computed in a neighborhood_size x neighborhood_size neighborhood
  // num_bins: number of bins in the histogram
  // grad_mag_on: can compute histogram distance based on the intensity values (false) or
  //    the gradient magnitude (true)
  // hist_method: "bhattacharyya", "jensen shannon" or "intersection" - specifies the
  //    desired comparison method
  int neighborhood_size;
  int num_bins;
  bool grad_mag_on;
  std::string hist_method;

  // Multi-image parameters
  // multi_method: multi-image fusion method ("product", "sum", "maximum")
  // pGoodness: probability of a given image being "good"
  bool multiple_ref;
  std::string multi_method;
  float pGoodness;

  //: Default parameters
  baml_change_detection_params() :
    method(NON_PARAMETRIC),
    correct_gain_offset(true),
    num_tiles(0),
    registration_refinement_rad(0),
    pChange(0.005),
    event_width(5),
    event_height(5),
    bt_rad(5),
    census_tol(10),
    census_rad(3),
    img_bit_depth(12),
    hist_bit_depth(10),
    neighborhood_size(11),
    num_bins(25),
    grad_mag_on(false),
    hist_method("bhattacharyya"),
    multiple_ref(true),
    multi_method("sum"),
    pGoodness(0.8) {}
};


//: Main class for computing change probabilities between a target image and
// reference image(s) using a variety of methods.
class baml_change_detection {

public:

  baml_change_detection(
    baml_change_detection_params  params ) :
      params_(std::move( params )){}

  //: Single reference image change detection
  bool detect(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& change_prob_target );

  //: Multi reference image change detection
  bool multi_image_detect(
    const vil_image_view<vxl_uint_16>& img_target,
    const std::vector<vil_image_view<vxl_uint_16> >& img_ref,
    const std::vector<vil_image_view<bool> >& valid,
    vil_image_view<float>& change_prob_target);

  //: Experimental: computes an image with each value corresponding to the
  // expected time of change at that pixel. (time is based on index of
  // reference image NOT on actual time)
  bool expected_time_change(
    const vil_image_view<vxl_uint_16>& img_target,
    const std::vector<vil_image_view<vxl_uint_16> >& img_ref,
    const std::vector<vil_image_view<bool> >& valid,
    vil_image_view<float>& change_time);

protected:

  baml_change_detection_params params_;

  //: Pairwise probability calculation
  // Used by both detect and multi_image_detect
  bool detect_internal(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& bg_prob,
    float& fg_prob);

  //: Multi-image alignment and pairwise probability calculation
  // Used by both multi_image_detect and expected_time_change
  bool detect_multi_internal(
    const vil_image_view<vxl_uint_16>& img_target,
    const std::vector<vil_image_view<vxl_uint_16> >& img_ref,
    const std::vector<vil_image_view<bool> >& valid,
    std::vector<vil_image_view<float> >& bg_probs,
    std::vector<float>& fg_probs,
    int& max_x_off,
    int& crop_width,
    int& max_y_off,
    int& crop_height);


  // ----------------------------Pairwise methods----------------------------
  //: All detect_<method> use the same paramerters
  //  img_target: target image (width x height)
  //  img_ref: reference image (width x height)
  //  valid: states whether pixel is valid or not (width x height)
  //  bg_prob: probability of change using <method> (width x height)
  //  fg_prob: foreground distribution, which is uniform based on the score

  //: Detect change using the Birchfield-Tomasi metric
  bool detect_bt(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& bg_prob,
    float& fg_prob );

  //: Detect change using census metric
  bool detect_census(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& bg_prob,
    float& fg_prob );

  //: Detect change using absolute difference
  bool detect_difference(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& bg_prob,
    float& fg_prob );

  //: Detect change using gradient
  bool detect_gradient(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid_ref,
    vil_image_view<float>& bg_prob,
    float& fg_prob );

  //: Detect change using the approximate mutual information method described in
  // the original semi-global matching paper
  bool detect_nonparam(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& bg_prob,
    float& fg_prob );

  //: Detect change using a distance measure between histograms of the
  // pixel values in the target and reference images
  bool detect_histcmp(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& bg_prob,
    float& fg_prob );


  // -----------------------Fusion methods-------------------------

  //: Fuse probability images using product method
  bool multi_product(
    const std::vector<vil_image_view<float> >& bg_probs,
    const std::vector<float>& fg_probs,
    vil_image_view<float>& change_prob);

  //: Fuse probability images using sum method
  bool multi_sum(
    const std::vector<vil_image_view<float> >& bg_probs,
    const std::vector<float>& fg_probs,
    vil_image_view<float>& change_prob);

  //: Fuse probability images using max method
  bool multi_max_prob(
    const std::vector<vil_image_view<float> >& bg_probs,
    const std::vector<float>& fg_probs,
    vil_image_view<float>& change_prob);


  // -----------------------Other methods-------------------------

  //: Histogram compare helper function
  bool build_hist(
    const vil_image_view<float>& img,
    const vil_image_view<int>& bin_img,
    const std::vector<double>& edges,
    const int x1,
    const int y1,
    const float step_size,
    const bool adding,
    std::vector <float>& hist);

  // Disable default/copy constructors
  baml_change_detection()= default;
  baml_change_detection( baml_change_detection& ){}
};


#endif // baml_detect_change_h_
