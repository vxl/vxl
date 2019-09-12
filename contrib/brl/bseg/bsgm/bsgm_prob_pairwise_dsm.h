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
// bsgm_multiscale_disparity_estimator mde(params_.de_params_, ni_, nj_, num_disparities(), num_active_disparities());
// mde.compute(rect_bview0_, rect_bview1_, invalid_map_, min_disparity_,
//             invalid_disp, params_.multi_scale_mode_, disp_r_);
//
// given a pixel (1182, 897) in rect_bview0 and the dispairity value at that location in the dispairity image,
// disp_r_, of -15.0, the corresponding pixel in rect_bview1 is (1167, 897)
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <bpgl/algo/bpgl_rectify_affine_image_pair.h>
#include <bsta/bsta_histogram.h>
#include "bsgm_disparity_estimator.h" // for disparity_estimator_params


struct pairwise_params{
  pairwise_params():
    active_disparity_factor_(0.5), downscale_exponent_(2), multi_scale_mode_(0),  //1
    point_sample_dist_(0.3f), upsample_scale_factor_(1.0f),
    std_dev_(3.75*point_sample_dist_), num_nearest_nbrs_(5), shadow_thresh_(20),
    use_z_vs_d_prob_(false), min_z_vs_d_scale_(1.0f), z_vs_d_std_dev_(1.0f),
    quad_interp_(false)
  {
    set_shadow_thresh(shadow_thresh_);  //default
    set_quad_interp(quad_interp_);
  }
  void set_shadow_thresh(float thresh){ de_params_.shadow_thresh = thresh; shadow_thresh_ = thresh;}
  void set_quad_interp(bool interp) {
    de_params_.perform_quadratic_interp=interp; quad_interp_ = interp;
  }
  bsgm_disparity_estimator_params de_params_; // internal disparity estimator params
  float active_disparity_factor_; // what fraction of full disparity range is used for fine search
  int downscale_exponent_;   // in coarse to fine disparity, what is the downsample ratio as 2^exponent
  int multi_scale_mode_;     // see disparity_estimator
  float point_sample_dist_;  // the height map grid spacing, also relates to consistent distance tolerance
  float upsample_scale_factor_; // upsample the rectified images by scale factor
  float std_dev_;            // the standard deviation of consistent disparity point distances
  bool use_z_vs_d_prob_;     // multiply height probabilty with additional z vs d scale probability factor
  float min_z_vs_d_scale_;   // the lowest z vs d scale factor that is typically obtained in meters/pixel
  float z_vs_d_std_dev_;     // the standard deviation for the z vs d Gaussian distribution
  size_t num_nearest_nbrs_;  // number of nearest neighbors in the pointset to find closest and to interpolate
  size_t shadow_thresh_;     // intensity level out of 255 below which is considered to be in shadow, thus invalid
  bool quad_interp_;         // if true, perform quadratic interpolation of disparity with respect to cost
};


class bsgm_prob_pairwise_dsm
{
 public:
  bsgm_prob_pairwise_dsm(): mid_z_(NAN), z_vs_disp_scale_(1.0) {}

  bsgm_prob_pairwise_dsm(vil_image_resource_sptr const& resc0, vpgl_affine_camera<double> const& acam0,
                         vil_image_resource_sptr const& resc1, vpgl_affine_camera<double> const& acam1):
    mid_z_(NAN), z_vs_disp_scale_(1.0)
  {
    rip_.set_images_and_cams(resc0, acam0, resc1, acam1);
  }

  bsgm_prob_pairwise_dsm(vil_image_view<unsigned char> const& view0, vpgl_affine_camera<double> const& acam0,
                         vil_image_view<unsigned char> const& view1, vpgl_affine_camera<double> const& acam1):
    mid_z_(NAN),z_vs_disp_scale_(1.0)
  {
    rip_.set_images_and_cams(vil_new_image_resource_of_view(view0), acam0, vil_new_image_resource_of_view(view1), acam1);
  }

  //: minimum dispartity to start search along an epipolar line
  void set_min_disparity(int min_disparity){min_disparity_ = min_disparity;}

  //: maximum dispartity to end search along an epipolar line
  void set_max_disparity(int max_disparity){max_disparity_ = max_disparity;}

  int min_disparity() const{return min_disparity_;}
  int max_disparity() const{return max_disparity_;}
  int num_disparities() { return (max_disparity_-min_disparity_); }

  //: how many disparity values are searched around the coarse search result
  int num_active_disparities() {
    return static_cast<int>(num_disparities()*params_.active_disparity_factor_); }

  //: set parameters
  void set_params(pairwise_params const& params){params_ = params;}

  //: retrieve current parameters
  pairwise_params params(){return params_;}

  //: set the plane elevation for minimum least squares disparity
  void set_midpoint_z(double mid_z){mid_z_ = mid_z;}
  //: estimate the forward disparities(arg order rectified image0:image1)
  bool compute_disparity();
  //: estimate the reverse disparities(arg order rectified image1:image0)
  bool compute_rev_disparity();

  //: compute height map and pointset, forward order only
  void compute_dsm_and_ptset(vgl_box_3d<double> const& scene_box);

  //: comptute forward and reverse heightmaps, pointsets and consistency filtered results
  bool compute_pointset_prob();
  void prob_heightmap(vgl_box_3d<double> const& scene_box);
  bool compute_dsm_and_ptset_prob(vgl_box_3d<double> const& scene_box);
  bool rect(vgl_box_3d<double> const& scene_box)
  {
    bool good = rip_.process(scene_box);
    if (good) {
      this->compute_byte();
    }
    return good;
  }

  //: main process method
  bool process(vgl_box_3d<double> const& scene_box, bool with_consistency_check = true)
  {
    rectify_params rp;
    rp.min_disparity_z_ = mid_z_;
    rp.upsample_scale_ = params_.upsample_scale_factor_;
    rip_.set_params(rp);
    if(!rip_.process(scene_box))
      return false;
    this->compute_byte();
    if(with_consistency_check){
      if(!compute_disparity())
        return false;
      if(!compute_rev_disparity())
        return false;
      if(!compute_dsm_and_ptset_prob(scene_box))
        return false;
      this->prob_heightmap(scene_box);
    }else{
      if(!compute_disparity())
        return false;
      compute_dsm_and_ptset(scene_box);
    }
    return true;
  }

  //: rectified images and cams
  const vil_image_view<vxl_byte>& rectified_bview0()const  {return rect_bview0_;}
  const vil_image_view<vxl_byte>& rectified_bview1()const  {return rect_bview1_;}
  const vpgl_affine_camera<double>& rectified_acam0()const {return rip_.rect_acam0();}
  const vpgl_affine_camera<double>& rectified_acam1()const {return rip_.rect_acam1();}

  //: disparity results
  vil_image_view<vxl_byte> invalid_map() const;
  vil_image_view<vxl_byte> rev_invalid_map() const;
  const vil_image_view<float>& disparity()const {return disp_r_;}
  const vil_image_view<float>& rev_disparity()const {return disp_r_reverse_;}

  //: triangulation results
  // uses rectified cams to reconstruct 3-d scene geometry
  // produces a 3-d pointset and a z(x,y) heightmap
  const vil_image_view<float>& heightmap()const {return height_map_;}
  const vil_image_view<float>& rev_heightmap()const {return height_map_reverse_;}
  const vil_image_view<float>& prob_heightmap_z()const {return prob_height_map_z_;}
  const vil_image_view<float>& prob_heightmap_prob()const { return prob_height_map_prob_; }
  const vgl_pointset_3d<float> ptset() const {return ptset_;}
  const vgl_pointset_3d<float> rev_ptset() const {return ptset_reverse_;}
  const vgl_pointset_3d<float> prob_ptset() const {return prob_ptset_;}
  bsta_histogram<float> prob_pdf() const {return prob_distr_;}

  //: apply a color map to the probabilty values and
  //  output a color point cloud as ascii
  bool save_prob_ptset_color(std::string const& path) const;

  // for debug purposes
  bool load_images_and_cams(std::string const& image0_path, std::string const& cam0_path,
                            std::string const& image1_path, std::string const& cam1_path)
  {
    return rip_.load_images_and_cams(image0_path, cam0_path, image1_path, cam1_path);
  }

 private:
  void compute_byte();
  bool z_vs_disparity_scale(double& scale) const;
  pairwise_params params_;
  bpgl_rectify_affine_image_pair rip_;
  size_t ni_;
  size_t nj_;
  int min_disparity_;
  int max_disparity_;
  double mid_z_;
  vil_image_view<vxl_byte> rect_bview0_;
  vil_image_view<vxl_byte> rect_bview1_;
  vil_image_view<bool> invalid_map_;
  vil_image_view<bool> invalid_map_reverse_;
  vil_image_view<float> tri_image_3d_;
  vil_image_view<float> disp_r_;
  vil_image_view<float> tri_image_3d_reverse_;
  vil_image_view<float> disp_r_reverse_;
  vil_image_view<float> height_map_;
  vil_image_view<float> height_map_reverse_;
  double z_vs_disp_scale_;
  vil_image_view<float> prob_height_map_z_;
  vil_image_view<float> prob_height_map_prob_;
  vgl_pointset_3d<float> ptset_;
  vgl_pointset_3d<float> ptset_reverse_;
  bsta_histogram<float> prob_distr_;
  vgl_pointset_3d<float> prob_ptset_;
};

#endif // bsgm_prob_pairwise_dsm_h_
