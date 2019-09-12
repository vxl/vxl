// This is//external/acal/acal_planar_feature_matcher.h
#ifndef acal_planar_feature_matcher_h
#define acal_planar_feature_matcher_h

//:
// \file
// \brief A class to construct custom features to match single images
// \author J.L. Mundy
// \date Dec 29, 2018
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <string>
#include <vector>
#include <map>
#include <math.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_affine_camera.h>


struct planar_feature_params
{
  planar_feature_params(): pt_spacing_(0.3), patch_radius_(6), coarse_search_radius_(20.0),
                           coarse_search_increment_(2.0), fine_search_radius_(2.5),
                           fine_search_increment_(0.25), max_n_solved_images_(10) {}

  double pt_spacing_;
  double patch_radius_;
  double coarse_search_radius_;
  double fine_search_radius_;
  double coarse_search_increment_;
  double fine_search_increment_;
  size_t max_n_solved_images_;
};


struct planar_match_score
{
  planar_match_score():mu_(0.67), sigma_(0.1), min_prob_(0.2) {}

  double p(double s) {
    return 0.5*(1.0+erf((s-mu_)/(sqrt(2)*sigma_)));
  }

  double mu_;
  double sigma_;
  double min_prob_;
  double min_score_;
  double max_score_;
  double mean_score_;
  double median_score_;
  double score_std_dev_;
};


class acal_planar_feature_matcher
{
 public:
  acal_planar_feature_matcher() {}
  acal_planar_feature_matcher(planar_feature_params const& params)
    {params_ = params;}

  // Load images/cams
  bool load_solved_images(std::map<size_t, std::string>& solved_image_paths);
  bool load_solved_affine_cams(std::map<size_t, std::string>& solved_aff_cam_paths);

  // Set images/cams
  void set_solved_affine_cams(std::map<size_t, vpgl_affine_camera<double> > solved_cams)
    {solved_cams_ = solved_cams;}
  void set_track_cam_ids(std::vector<size_t> const& track_cams)
    {track_cams_ = track_cams;}
  void set_track_3d_points(std::map<size_t, vgl_point_3d<double> > const& track_3d_pts)
    {track_3d_pts_ = track_3d_pts;}
  planar_feature_params params()
    {return params_;}

  void backproject_patch(vil_image_view<vxl_byte> const& view, vpgl_affine_camera<double> const& cam,
                         vgl_point_3d<double> const& p3d, vil_image_view<vxl_byte>& patch, double u_off=0.0, double v_off=0.0);
  bool generate_image_patches();

  //: apply a grid search for a local maxium in patch appearance score
  bool local_maximum(vgl_vector_2d<double> grid_center, double grid_increment, double radius,
	  vil_image_view<vxl_byte>  const& ref_patch, vgl_point_3d<double> const& p3d,
	  vil_image_view<vxl_byte>  const& img, vpgl_affine_camera<double> const& cam,
	  vgl_vector_2d<double>& max_location, double& max_score);

  bool set_match_translations(size_t cam_id,vil_image_view<vxl_byte>  const& img, vpgl_affine_camera<double> const& cam);

  //: find the median u and median v values over the set of
  //  translations for the set of solved cameras and tracks
  void compute_scores_stats_median_trans();

  bool process(size_t cam_id,vil_image_view<vxl_byte>  const& img, vpgl_affine_camera<double> const& cam);

  vgl_vector_2d<double> median_translation() { return median_translation_; }
  planar_match_score match_score_stats(){return match_score_stats_;}

  // debug
  bool solved_cam(size_t cam_id, vpgl_affine_camera<double>& cam){
    if(solved_cams_.count(cam_id) == 0){
      std::cout << cam_id << " is not in the set of solved cameras"<< std::endl;
      return false;
    }
    cam = solved_cams_[cam_id];
    return true;
  }
  bool save_planar_patches(std::string const& patch_dir);
  bool save_ground_truth_test_patch(size_t pt_id, std::string const& patch_dir, vil_image_view<vxl_byte>  const& img, vpgl_affine_camera<double> const& cam, double du, double dv);
  void print_matches();

 private:
  planar_feature_params params_;
  std::map<size_t, vil_image_view<vxl_byte> > solved_images_;
  std::map<size_t, vpgl_affine_camera<double> > solved_cams_;
  std::vector<size_t> track_cams_;
  std::map<size_t, vgl_point_3d<double> > track_3d_pts_;
  std::vector<std::map<size_t, vil_image_view<vxl_byte> > > planar_patches_;
  std::vector<std::map<size_t, std::pair<vgl_vector_2d<double>, double> > > cam_translations_;
  vgl_vector_3d<double>  test_cam_ray_dir_;
  planar_match_score match_score_stats_;
  vgl_vector_2d<double> median_translation_;

};

#endif
