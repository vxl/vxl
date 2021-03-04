// This is brl/bseg/bsgm/bsgm_prob_pairwise_dsm.h
#ifndef bsgm_prob_pairwise_dsm_h
#define bsgm_prob_pairwise_dsm_h

//:
// \file
// \brief A class to provide a convenient interface to compute a pairwise dsm
// \author J.L. Mundy
// \date February 11, 2019
//
// \verbatim
//  Modifications
//   Sept. 3, 2019
//   JLM - added additional probability factor based on the rate of change in z with respect to disparity
// \endverbatim
// uses the SGM algorithm to compute a pair of disparity images, normal and reverse.
// the reverse disparity image is computed by simply flipping the order of the
// images passed to the SGM algorithm. The forward and reverse disparity images are
// converted to a 3-d pointset by finding the 3-d points that produce the observed
// disparities.  A probability is applied to each forward 3-d point based on the 3-d distance
// between a forward point and the closest point from the reverse reconstruction
//
// There can be different conventions concerning the sign of disparity values
// In the case of bsgm_disparity_estimator, disparity is defined as the horizontal translation
// required to move from a pixel in the reference image to the corresponding pixel in the
// target image.

//In more detail, consider the following call to the multi-scale estimator:
//
// bsgm_multiscale_disparity_estimator mde(params_.de_params_, rect_ni_, rect_nj_, num_disparities(), num_active_disparities());
// mde.compute(rect_bview0_, rect_bview1_, invalid_map_, min_disparity_,
//             invalid_disp, params_.multi_scale_mode_, disp_r_);
//
// given a pixel (1182, 897) in rect_bview0 and the disparity value at that location in the disparity image,
// disp_r_, of -15.0, the corresponding pixel in rect_bview1 is (1167, 897)
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vul/vul_timer.h>
#include <bpgl/algo/bpgl_rectify_image_pair.h>
#include <bpgl/algo/bpgl_heightmap_from_disparity.h>
#include <bpgl/algo/bpgl_gridding.h>
#include <bsta/bsta_histogram.h>
#include "bsgm_disparity_estimator.h" // for disparity_estimator_params


struct pairwise_params
{

  // constructor
  pairwise_params()
  {
    shadow_thresh(shadow_thresh_);
    quad_interp(quad_interp_);
  }

  // accessors
  void shadow_thresh(float thresh) {
    de_params_.shadow_thresh = thresh;
    shadow_thresh_ = thresh;
  }

  void quad_interp(bool interp) {
    de_params_.perform_quadratic_interp = interp;
    quad_interp_ = interp;
  }

  // internal disparity estimator params
  bsgm_disparity_estimator_params de_params_;

  // intensity level below which is considered to be in shadow, thus invalid
  size_t shadow_thresh_ = 20;

  // if true, perform quadratic interpolation of disparity with respect to cost
  bool quad_interp_ = false;

  // see disparity_estimator
  int multi_scale_mode_ = 0;

  // what fraction of full disparity range is used for fine search
  float active_disparity_factor_ = 0.5f;

   // in coarse to fine disparity, what is the downsample ratio as 2^exponent
  int downscale_exponent_ = 2;

  // the height map grid spacing, also relates to consistent distance tolerance
  float ground_sample_dist_ = 0.3f;

  // reverse the sign of disparity (disparity_sense_ = -1) for situations where the scene is behind the camera
  // happens occasionally for perspective cameras computed by SfM
  int disparity_sense_ = 1;

  // upsample the rectified images by scale factor
  float upsample_scale_factor_ = 1.0f;

  // minimum fraction of points in the zero disparity plane (w/in scene box) that must project into both images
  double min_overlap_fraction_ = 0.25;

  // the standard deviation of consistent disparity point distances
  float std_dev_ = 3.75*ground_sample_dist_;

  // multiply height probability with additional z vs d scale probability factor
  bool use_z_vs_d_prob_ = false;

  // the lowest z vs d scale factor that is typically obtained in meters/pixel
  float min_z_vs_d_scale_ = 1.0f;

  // the standard deviation for the z vs d Gaussian distribution
  float z_vs_d_std_dev_ = 1.0f;

  // pointset->heightmap gridding parameters:
  // expected number of neighbors (between min/max neighbors) within some distance
  // (_neighbor_dist_factor * _ground_sample_distance) of each heightmap pixel
  unsigned min_neighbors_ = 3;
  unsigned max_neighbors_ = 5;
  float neighbor_dist_factor_ = 3.0;

  //use the reduced resolution dsm to estimate min disparity
  //num_active_disparities are used if true otherwise num_disparities
  bool coarse_dsm_disparity_estimate_ = true;

  // the effective bits per pixel for an image, e.g. a dynamic range of 2047 corresponds to 11 bits
  int effective_bits_per_pixel_ = 8;

  int window_padding_ = 100;
};


template <class CAM_T, class PIX_T>
class bsgm_prob_pairwise_dsm
{
 public:
  bsgm_prob_pairwise_dsm()
  {
    init_dynamic_range_table();
    H0_.fill(NAN);
    H1_.fill(NAN);
  }

  bsgm_prob_pairwise_dsm(vil_image_view_base_sptr const& view0, CAM_T const& cam0,
                         vil_image_view_base_sptr const& view1, CAM_T const& cam1)
  {
    set_images_and_cams(view0, cam0, view1, cam1);
    init_dynamic_range_table();
    H0_.fill(NAN);
    H1_.fill(NAN);
  }

  bsgm_prob_pairwise_dsm(vil_image_resource_sptr const& resc0, CAM_T const& cam0,
                         vil_image_resource_sptr const& resc1, CAM_T const& cam1)
  {
    set_images_and_cams(resc0, cam0, resc1, cam1);
    init_dynamic_range_table();
    H0_.fill(NAN);
    H1_.fill(NAN);
  }

  bsgm_prob_pairwise_dsm(vil_image_view<PIX_T> const& view0, CAM_T const& cam0,
                         vil_image_view<PIX_T> const& view1, CAM_T const& cam1)
  {
    set_images_and_cams(view0, cam0, view1, cam1);
    init_dynamic_range_table();
    H0_.fill(NAN);
    H1_.fill(NAN);
  }

  // ACCESSORS-----

  //: set images & cameras for analysis
  void set_images_and_cams(vil_image_view_base_sptr const& view0, CAM_T const& cam0,
                           vil_image_view_base_sptr const& view1, CAM_T const& cam1)
  {
    rip_.set_images(view0, view1);
    this->set_cameras(cam0, cam1);
  }

  void set_images_and_cams(vil_image_view<PIX_T> const& view0, CAM_T const& cam0,
                           vil_image_view<PIX_T> const& view1, CAM_T const& cam1)
  {
    vil_image_resource_sptr resc0_ptr = vil_new_image_resource_of_view(view0);
    vil_image_resource_sptr resc1_ptr = vil_new_image_resource_of_view(view1);
    rip_.set_images(resc0_ptr, resc1_ptr);
    this->set_cameras(cam0, cam1);
  }

  void set_images_and_cams(vil_image_resource_sptr const& resc0, CAM_T const& cam0,
                           vil_image_resource_sptr const& resc1, CAM_T const& cam1)
  {
    rip_.set_images(resc0, resc1);
    this->set_cameras(cam0, cam1);
  }

  void set_cameras(CAM_T const& cam0, CAM_T const& cam1)
  {
    rip_.set_cameras(cam0, cam1);
    cam0_ = cam0;
    cam1_ = cam1;
  }

  //: set a table of scale factors with respect to the effective bits per pixel
  //  of the input imagery. For example the actual bits per pixel might be 11 or a range of (0, 2047)
  //  however the typical range of intensities might be only (0, 725) so the appearance cost scale
  //  will be 725/255 ~ 2.8. The nominal parameters in bsgm are tuned for byte images thus the 255 denominator.
  //  census threshold and gradient magnitude are adjusted for higher dynamic ranges according to this scale factor
  void set_dynamic_range_table(std::map<int, float> const& bits_per_pix_factors){
    bits_per_pix_factors_ = bits_per_pix_factors;
  }
  //: parameters
  void params(pairwise_params const& params) {params_ = params;}
  pairwise_params params() const { return params_; }

  //: minimum disparity to start search along an epipolar line
  void min_disparity(int min_disparity) {min_disparity_ = min_disparity;}
  int min_disparity() const { return min_disparity_; }

  //: maximum disparity to end search along an epipolar line
  void max_disparity(int max_disparity) {max_disparity_ = max_disparity;}
  int max_disparity() const { return max_disparity_; }

  //: number of disparities
  int num_disparities() const { return (max_disparity_ - min_disparity_); }

  //: how many disparity values are searched around the coarse search result
  int num_active_disparities() const {
    return static_cast<int>(num_disparities()*params_.active_disparity_factor_);
  }

  //: plane elevation for minimum least squares disparity
  void midpoint_z(double mid_z) {mid_z_ = mid_z;}
  double midpoint_z() const { return mid_z_; }

  //: scene box for analysis
  void scene_box(vgl_box_3d<double> scene_box) { scene_box_ = scene_box; }
  vgl_box_3d<double> scene_box() const { return scene_box_; }

  //: H0 rectification matrix (allow setting if already pre-computed)
  void H0(vnl_matrix_fixed<double, 3, 3> H0) {H0_ = H0;}
  vnl_matrix_fixed<double, 3, 3> H0() const {return H0_;}

  //: H1 rectification matrix (allow setting if already pre-computed)
  void H1(vnl_matrix_fixed<double, 3, 3> H1) {H1_ = H1;}
  vnl_matrix_fixed<double, 3, 3> H1() const {return H1_;}

  //: rectification image width (allow setting if already pre-computed)
  void rect_ni(size_t rect_ni) {rect_ni_ = rect_ni;}
  size_t rect_ni() const {return rect_ni_;}

  //: rectification image height (allow setting if already pre-computed)
  void rect_nj(size_t rect_nj) {rect_nj_ = rect_nj;}
  size_t rect_nj() const {return rect_nj_;}

  //: target image window to process within
  void target_window(vgl_box_2d<int> target_window) { target_window_ = target_window; }
  vgl_box_2d<int> target_window() const { return target_window_; }

  //: rectified images and cams
  const vil_image_view<PIX_T>& rectified_bview0() const {return rect_bview0_;}
  const vil_image_view<PIX_T>& rectified_bview1() const {return rect_bview1_;}
  const CAM_T& rectified_cam0() const {return rect_cam0_;}
  const CAM_T& rectified_cam1() const {return rect_cam1_;}
  const vgl_box_2d<int>& rectified_target_window() const {return rect_target_window_;}
  const vgl_box_2d<int>& rectified_reference_window() const {return rect_reference_window_;}

  //: disparity results
  vil_image_view<vxl_byte> invalid_map_fwd() const { return bool_to_byte(invalid_map_fwd_); }
  vil_image_view<vxl_byte> invalid_map_rev() const { return bool_to_byte(invalid_map_rev_); }
  const vil_image_view<float>& disparity_fwd() const {return disparity_fwd_;}
  const vil_image_view<float>& disparity_rev() const {return disparity_rev_;}

  //: triangulation results
  // uses rectified cams to reconstruct 3-d scene geometry
  // produces a 3-d pointset and a z(x,y) heightmap
  const vil_image_view<float>& tri_3d_fwd() const {return tri_3d_fwd_;}
  const vil_image_view<float>& tri_3d_rev() const {return tri_3d_rev_;}
  const vil_image_view<float>& xyz_prob() const {return xyz_prob_;}

  const vil_image_view<float>& heightmap_fwd() const {return heightmap_fwd_;}
  const vil_image_view<float>& heightmap_rev() const {return heightmap_rev_;}

  const vgl_pointset_3d<float> ptset_fwd() const {return ptset_fwd_;}
  const vgl_pointset_3d<float> ptset_rev() const {return ptset_rev_;}

  //: probabilistic results
  const vgl_pointset_3d<float> prob_ptset() const {return prob_ptset_;}
  bsta_histogram<float> prob_pdf() const {return prob_distr_;}

  const vil_image_view<float>& prob_heightmap() const {return prob_heightmap_z_;}
  const vil_image_view<float>& prob_confidence() const { return prob_heightmap_prob_; }
  const vil_image_view<float>& radial_std_dev_image() const {return radial_std_dev_image_; }

  // PROCESS-----

  //: image rectification
  void rectify();

  //: compute disparities
  // fwd: arg order rectified image0:image1
  // rev: arg order rectified image1:image0
  void compute_disparity_fwd();
  void compute_disparity_rev();

  //: compute height data (tri_3d, ptset, heightmap)
  void compute_height_fwd(bool compute_hmap);
  void compute_height_rev(bool compute_hmap);

  //: compute probabilistic height
  bool compute_prob(bool compute_prob_heightmap = true);

  //: compute xyz_prob image - a 4 plane image of coordinates and probability
  //  image planes: 0 -> x, 1 -> y, 2 -> z, 3 -> prob
  void compute_xyz_prob(bool compute_heightmap = true);

  //: main process method
  // with consistency check both forward and reverse disparities are computed
  // and resulting 3-d points checked for consistency. knn_consistency matches
  // forward and reverse 3-d points using a kd-tree index. Otherwise,
  // 3-d points are matched according to the disparity location in
  // the reverse xyz image
  bool process(bool with_consistency_check = true, bool knn_consistency = true,
               bool compute_fwd_rev_ptsets_hmaps = true)
  {
    // check if not in window mode
    bool window_mode = !target_window_.is_empty();
    if (window_mode) {
      throw std::runtime_error("Can't apply window processing using this process method");
      return false;
    }
    // rectification
    this->rectify();

    // compute forward disparity & height
    this->compute_disparity_fwd();
    this->compute_height_fwd(compute_fwd_rev_ptsets_hmaps);

    // consistency check & probabilistic analysis
    if (with_consistency_check) {
      this->compute_disparity_rev();
      this->compute_height_rev(compute_fwd_rev_ptsets_hmaps);

      if (knn_consistency) {
        if (!compute_prob(true))  // true -> compute prob heightmap
          return false;
      } else {
        this->compute_xyz_prob(true);  // true -> compute prob heightmap
      }
    } else this->compute_ptset();

    return true;
  }


  //: main process method when using windows into the full target and reference images
  // the windows are defined in the original images not the rectified images
  // the specified windows are transformed by the rectifying transforms.
  // the flag first window is controlled by the user to allow multiple windows to be
  // processed using the same rectified image pair. If first_window is true, then
  // rectification is performed otherwise only stereo processing is carried out.
  // Note that in the window mode, only the 3-d pointset with a probability scalar is
  // computed -- no heightmaps are produced. If with_consistency_check is false then
  // no point probabilities are computed and the value is always 1.1 indicating invalid.
  bool process_with_windows(bool& first_window = true,
                            bool with_consistency_check = false,
                            bool print_timing = false)
  {
    // check if not in window mode
    bool not_window_mode = target_window_.is_empty();

    /* if (not_window_mode) { */
    /*   throw std::runtime_error("Can't apply window processing without target window set"); */
    /*   return false; */
    /* } */

    // tell disparity estimator to also print timing estimates
    params_.de_params_.print_timing = print_timing;

    vul_timer t;
    // rectification
    if (first_window) {
      this->rectify();
      if (print_timing) {
        std::cout << "rectification(" << rect_bview0_.ni() << " x "
                  << rect_bview0_.nj() << ") in " << t.real() / 1000.0
                  << " sec." << std::endl;
      }
    } else {
      prob_ptset_.clear();
      rectify_windows();
    }
    // compute forward disparity & height
    this->compute_disparity_fwd();
    this->compute_height_fwd(false);  // false -> don't compute fwd heightmap
    if (print_timing)
        std::cout << "forward disparity and xyz img in " << t.real() / 1000.0 << " sec." << std::endl;
    if (with_consistency_check) {
      // consistency check & probabilistic analysis
      t.mark();
      this->compute_disparity_rev();
      this->compute_height_rev(false);  // false -> don't compute reverse heightmap
      this->compute_xyz_prob(false);  // false->don't compute heightmap, just prob pointset
      if (print_timing)
        std::cout << "reverse disparity and prob ptset in " << t.real() / 1000.0 << " sec." << std::endl;
    } else {
      this->compute_ptset();
      if (print_timing)
        std::cout << "non probabilistic ptset in " << t.real() / 1000.0 << " sec." << std::endl;
    }
    return true;
  }

  // MISC-----

  //: apply a color map to the probability values and
  //  output a color point cloud as ascii
  bool save_prob_ptset_color(std::string const& path) const;

 protected:

  vgl_box_2d<int> rectify_window(
      vgl_box_2d<int>& window,
      vnl_matrix_fixed<double, 3, 3> H,
      size_t ni, size_t nj);

  void translate_camera_into_window(
      CAM_T& cam, vgl_box_2d<int> window);

  void rectify_windows();

  // compute disparity for generic inputs
  void compute_disparity(
      const vil_image_view<PIX_T>& img,
      const vil_image_view<PIX_T>& img_reference,
      bool forward,  // == true or reverse == false
      vil_image_view<bool>& invalid,
      vil_image_view<float>& disparity,
      vgl_box_2d<int>& img_window,
      vgl_box_2d<int>& img_reference_window);

  // compute height for generic inputs
  void compute_height(
    const CAM_T& cam,
    const CAM_T& cam_reference,
    const vil_image_view<float>& disparity,
    vil_image_view<float>& tri_3d,
    vgl_pointset_3d<float>& ptset,
    vil_image_view<float>& heightmap);

  // z vs disparity scale
  bool z_vs_disparity_scale(double& scale) const;

  // boolean to byte image conversion
  vil_image_view<vxl_byte> bool_to_byte(const vil_image_view<bool>& img) const;

  // get bpgl_heightmap instance
  bpgl_heightmap<float> get_bpgl_heightmap() const;

  // compute the pointset without consistency checks
  // probability = 1.1f to indicate no check was done
  // and mitigate effects on algorithms using the result
  void compute_ptset();

 private:

  //: fill default dynamic range scale factors
  void init_dynamic_range_table(){
    bits_per_pix_factors_[8] = 1.0f;
    bits_per_pix_factors_[11] = 2.8f;
  }
  bool affine_;  // vs. perspective
  pairwise_params params_;
  CAM_T cam0_;
  CAM_T cam1_;
  CAM_T rect_cam0_;
  CAM_T rect_cam1_;
  bpgl_rectify_image_pair<CAM_T> rip_;

  size_t rect_ni_;
  size_t rect_nj_;
  int min_disparity_ = -100;
  int max_disparity_ = 100;

  double mid_z_ = NAN;
  double z_vs_disp_scale_ = 1.0;

  vgl_box_3d<double> scene_box_;

  // rectification matrices
  vnl_matrix_fixed<double, 3, 3> H0_;
  vnl_matrix_fixed<double, 3, 3> H1_;

  vgl_box_2d<int> target_window_;
  vgl_box_2d<int> rect_target_window_;

  vgl_box_2d<int> rect_reference_window_;

  vil_image_view<PIX_T> rect_bview0_;
  vil_image_view<PIX_T> rect_bview1_;

  std::map<int, float> bits_per_pix_factors_;

  // disparity data
  vil_image_view<bool> invalid_map_fwd_;
  vil_image_view<bool> invalid_map_rev_;
  vil_image_view<float> disparity_fwd_;
  vil_image_view<float> disparity_rev_;

  // triangulated data
  vil_image_view<float> tri_3d_fwd_;
  vil_image_view<float> tri_3d_rev_;
  vil_image_view<float> xyz_prob_;

  // height data
  vgl_pointset_3d<float> ptset_fwd_;
  vgl_pointset_3d<float> ptset_rev_;

  vil_image_view<float> heightmap_fwd_;
  vil_image_view<float> heightmap_rev_;

  // probabilistic data
  vgl_pointset_3d<float> prob_ptset_;
  bsta_histogram<float> prob_distr_;

  vil_image_view<float> prob_heightmap_z_;
  vil_image_view<float> prob_heightmap_prob_;
  vil_image_view<float> radial_std_dev_image_;
};
#define BSGM_PROB_PAIRWISE_DSM_INSTANTIATE(CAM_T, PIX_T) extern "please include bsgm/bsgm_prob_pairwise_dsm.hxx first"
#endif // bsgm_prob_pairwise_dsm_h_
