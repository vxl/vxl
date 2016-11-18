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


//:
// \file
// \brief 
// \author Thomas Pollard
// \date August 22, 2016


//: Assorted pairwise image change detection algorithms.
enum baml_change_detection_method { 
  BIRCHFIELD_TOMASI, 
  CENSUS,
  GRADIENT_DIFF,
  NON_PARAMETRIC
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

  // Non-parametric params
  int img_bit_depth;
  int hist_bit_depth;

  //: Default parameters
  baml_change_detection_params():
	method(BIRCHFIELD_TOMASI),
    correct_gain_offset( true ),
    num_tiles( 3 ),
    registration_refinement_rad( 0 ),
    prior_change_prob( 0.01f ),
    bt_std( 20.0f ),
    bt_rad( 1 ),
    census_std( 0.3f ),
    census_tol( 10 ),
    census_rad( 3 ),
    grad_std( 30.0f ),
    img_bit_depth( 12 ),
    hist_bit_depth( 10 ){}
};


//: Main class for computing change probabilities between pairs of images 
// using a variety of methods.
class baml_change_detection {

public:

  baml_change_detection(
    const baml_change_detection_params& params ) :
      params_( params ){}

  bool detect(
    const vil_image_view<vxl_uint_16>& img_target,
    const vil_image_view<vxl_uint_16>& img_ref,
    const vil_image_view<bool>& valid,
    vil_image_view<float>& change_prob_target );

protected:

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

  // Disable default/copy constructors
  baml_change_detection(){}
  baml_change_detection( baml_change_detection& ){}
};


#endif // baml_detect_change_h_
