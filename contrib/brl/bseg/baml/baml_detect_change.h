// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef baml_detect_change_h_
#define baml_detect_change_h_

#include <string>
#include <vector>
#include <set>
#include <utility>

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
  float prior_change_prob;

  // Birchfield-Tomasi specific parameters
  float bt_std;
  int bt_rad;

  // Census parameters
  // census_tol: pixel differences less than this magnitude are not considered
  //   in the census computation.  Increase to prevent errors from sensor noise. 
  //   Set to 0 for textbook census implementation.
  // census_rad: length of the census kernal will be 2*census_rad+1. Must be 
  //    1,2,or 3.
  float census_std;
  int census_tol;
  int census_rad; 

  // Gradient parameters
  float grad_std;

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
  // multi_method: multi-image fusion method ("product", "sum", "minimum")
  // pGoodness: probability of a given image being "good"
  // pChange: probability of any given pixel being a changed pixel 
  std::string multi_method;
  float pGoodness;
  float pChange;

  //: Default parameters
  baml_change_detection_params() :
    method(NON_PARAMETRIC),
    correct_gain_offset(true),
    num_tiles(0),
    registration_refinement_rad(0),
    prior_change_prob(0.01f), // try increasing this for BT 0.01f
    bt_std(20.0f), // or this 20.0f
    bt_rad(1),
    census_std(0.3f),
    census_tol(10),
    census_rad(3),
    grad_std(3.0f), // 30.0f
    img_bit_depth(12),
    hist_bit_depth(10),
    neighborhood_size(11),
    num_bins(25), 
    grad_mag_on(false),
    hist_method("bhattacharyya"),
    multi_method("sum"),
    pGoodness(0.8),
    pChange(0.005) {}
};


//: Main class for computing change probabilities between pairs of images 
// using a variety of methods.
class baml_change_detection {

public:

  baml_change_detection(
    const baml_change_detection_params& params ) :
      params_( params ){}

  //: Single refrence image change detection
  // calls detect_internal to do score calculation
  // then converts to probability with sigmoid
  bool detect(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& change_prob_target );

  //: Multi reference image change detection
  // calls detect_internal to do pairwise score calculations
  // then uses a multi-image fusion technique
  bool baml_change_detection::multi_image_detect(
    const vil_image_view<vxl_uint_16>& img_target,
    const std::vector<vil_image_view<vxl_uint_16>> img_ref,
    const std::vector<vil_image_view<bool>> valid,
    vil_image_view<float>& change_prob_target);

protected:
  
  //: Used by both detect and multi_image_detect
  // pairwise score calculation functionality contained in this function
  bool detect_internal(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& change_prob_target);

  baml_change_detection_params params_;
  
  //: Detect change using the Birchfield-Tomasi metric
  bool detect_bt(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& tar_lh );

  //: Detect change using census metric
  bool detect_census(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& tar_lh );

  //: Detect change using absolute difference
  bool detect_difference(const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& tar_lh);

  //: Detect change using gradient
  bool detect_gradient(
    const vil_image_view<vxl_uint_16>& img_tar,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid_ref,
    vil_image_view<float>& tar_lh );

  //: Detect change using the approximate mutual information method described in
  // the original semi-global matching paper
  bool detect_nonparam(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& tar_lh );

  //: Detect change using a distance measure between histograms of the 
  // pixel values in the target and reference images
  bool detect_histcmp(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& tar_lh);
  // -------------------histogram compare helper function-------------------------
  bool
    baml_change_detection::build_hist(
      const vil_image_view<float>& img,
      const vil_image_view<int>& bin_img,
      const std::vector<double>& edges,
      const int x1,
      const int y1,
      const float step_size,
      const bool adding,
      std::vector <float>& hist);


  //: Fuse score images using product method
  bool 
    baml_change_detection::multi_product(
    const std::vector<vil_image_view<float>> lh,
    float sigma,
    vil_image_view<float>& probability
  );

  //: Fuse score images using sum method
  bool 
    baml_change_detection::multi_sum(
    const std::vector<vil_image_view<float>> lh,
    float sigma,
    vil_image_view<float>& probability
  );

  //: Fuse score images using score minization method
  bool baml_change_detection::multi_min(
    const std::vector<vil_image_view<float>> scores,
    vil_image_view<float>& probability
  );

  // Disable default/copy constructors
  baml_change_detection(){}
  baml_change_detection( baml_change_detection& ){}
};


#endif // baml_detect_change_h_
