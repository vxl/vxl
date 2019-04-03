// This is brl/bseg/bsgm/bsgm_pairwise_dsm.h
#ifndef bsgm_pairwise_dsm_h_
#define bsgm_pairwise_dsm_h_

//:
// \file
// \brief A class to provide a convenient interface to compute a pairwise dsm
// \author J.L. Mundy
// \date February 11, 2019
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
// uses the SGM algorithm to compute a pair of disparity images, normal and reverse.
// the reverse disparity image is computed by simply flipping the order of the
// images passed to the SGM algorithm. The forward and reverse disparity images are
// converted to a 3-d pointset by finding the 3-d points that produce the observed
// disparities.  The forward and reverse pointsets are then pruned to remove any
// point in the forward pointset that does not have a nearby point in the reverse pointset.
// This consistency check is done in 3-d coordinates since the 3-d distance between points
// is invariant to camera viewpoint while image disparity distances are not. For example,
// the tolerance for consistent disparities will be much tighter for image pairs that have
// nearly the same viewpoint.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vil/vil_image_view.h>
#include <bpgl/algo/bpgl_rectify_affine_image_pair.h>
#include "bsgm_disparity_estimator.h" // for disparity_estimator_params


struct pairwise_params{
  pairwise_params():active_disparity_factor_(0.5),downscale_exponent_(2), multi_scale_mode_(1),
    point_sample_dist_(0.5f), max_consist_dist_(3.75*point_sample_dist_), num_nearest_nbrs_(5),
    avg_z_(50.0), npts_(1000){}

  bsgm_disparity_estimator_params de_params_; // internal disparity estimator params
  float active_disparity_factor_; // what fraction of full disparity range is used for fine search
  int downscale_exponent_;   // in coarse to fine disparity, what is the downsample ratio as 2^exponent
  int multi_scale_mode_;     // see disparity_estimator
  float point_sample_dist_;  // the height map grid spacing, also relates to consistent distance tolerance
  float max_consist_dist_;   // the max distance between consistent forward/reverse 3-d points
  size_t num_nearest_nbrs_;  // number of nearest neighbors in the pointset to find closest and to interpolate
  double avg_z_;        // the averge scene height where disparity == 0
  size_t npts_;              // the number of points to use as rectification correspondences
};


class bsgm_pairwise_dsm
{
 public:
  bsgm_pairwise_dsm(){}

  bsgm_pairwise_dsm(vil_image_resource_sptr const& resc0, vpgl_affine_camera<double> const& acam0,
                      vil_image_resource_sptr const& resc1, vpgl_affine_camera<double> const& acam1)
  {
    rip_.set_images_and_cams(resc0, acam0, resc1, acam1);
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

  //: estimate the forward disparities(arg order rectified image0:image1)
  bool compute_disparity();
  //: estimate the reverse disparities(arg order rectified image1:image0)
  bool compute_rev_disparity();

  //: compute height map and pointset, forward order only
  void compute_dsm_and_ptset(vgl_box_3d<double> const& scene_box);

  //: comptute forward and reverse heightmaps, pointsets and consistency filtered results
  bool consistent_pointset();
  void consistent_heightmap(vgl_box_3d<double> const& scene_box);
  bool compute_consistent_dsm_and_ptset(vgl_box_3d<double> const& scene_box);

  //: main process method
  bool process(vgl_box_3d<double> const& scene_box, bool with_consistency_check = true)
  {
    if(!rip_.process(scene_box, params_.npts_, params_.avg_z_))
      return false;
    this->compute_byte();
    if(with_consistency_check){
      if(!compute_disparity())
        return false;
      if(!compute_rev_disparity())
        return false;
      if(!compute_consistent_dsm_and_ptset(scene_box))
        return false;
      this->consistent_heightmap(scene_box);
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
  const vil_image_view<float>& consist_heightmap()const {return consist_height_map_;}
  const vgl_pointset_3d<float> ptset() const {return ptset_;}
  const vgl_pointset_3d<float> rev_ptset() const {return ptset_reverse_;}
  const vgl_pointset_3d<float> consist_ptset() const {return consist_ptset_;}

  // for debug purposes
  bool load_images_and_cams(std::string const& image0_path, std::string const& cam0_path,
                            std::string const& image1_path, std::string const& cam1_path)
  {
    return rip_.load_images_and_cams(image0_path, cam0_path, image1_path, cam1_path);
  }

 private:
  void compute_byte();
  pairwise_params params_;
  bpgl_rectify_affine_image_pair rip_;
  size_t ni_;
  size_t nj_;
  int min_disparity_;
  int max_disparity_;
  vil_image_view<vxl_byte> rect_bview0_;
  vil_image_view<vxl_byte> rect_bview1_;
  vil_image_view<bool> invalid_map_;
  vil_image_view<bool> invalid_map_reverse_;
  vil_image_view<float> disp_r_;
  vil_image_view<float> disp_r_reverse_;
  vil_image_view<float> height_map_;
  vil_image_view<float> height_map_reverse_;
  vil_image_view<float> consist_height_map_;
  vgl_pointset_3d<float> ptset_;
  vgl_pointset_3d<float> ptset_reverse_;
  vgl_pointset_3d<float> consist_ptset_;
};

#endif // bsgm_pairwise_dsm_h_
