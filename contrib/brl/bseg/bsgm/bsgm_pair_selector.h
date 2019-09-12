// This is//terra/bsgm_pair_selector.h
#ifndef bsgm_pair_selector_h
#define bsgm_pair_selector_h

//:
// \file
// \brief A class to order image pairs according to likely dsm quality
// \author J.L. Mundy
// \date April 6, 2019
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <limits>
#include <vpgl/vpgl_affine_camera.h>
#include <bpgl/acal/acal_f_utils.h> // for dtime
#include <bpgl/acal/acal_metadata.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>

struct bsgm_pair_info
{
  bsgm_pair_info() {}
  size_t i_;
  size_t j_;
  std::string iname_i_;
  std::string iname_j_;
  double view_angle_diff_;
  double sun_angle_diff_;
  double gsd_ratio_;
  double cost_;
};

class bsgm_pair_less
{
public:
  bsgm_pair_less(double best_angle, double low_angle_bnd, double high_angle_bnd, double max_sun_ang, double max_gsd_ratio,
                 std::map<size_t, std::map<size_t, double> > sun_angle_diffs,
                 std::map<size_t, std::map<size_t, double> >& ang_diffs,
                 std::map<size_t, std::map<size_t, double> >& gsd_ratios)
    : best_angle_(best_angle), low_angle_bnd_(low_angle_bnd), high_angle_bnd_(high_angle_bnd),
      max_sun_ang_(max_sun_ang), max_gsd_ratio_(max_gsd_ratio),
      sun_angle_diffs_(sun_angle_diffs), ang_diffs_(ang_diffs), gsd_ratios_(gsd_ratios) {}

  //: a function to represent an upper bound cost
  static double sc(double x, double x_half) {
    double den = pow(fabs(x_half), 4);
    if(den == 0.0)
      return std::numeric_limits<double>::max();
    double temp = pow(fabs(x),4);
    temp /= den;
    return temp + 0.1;// keep a small floor to the cost to avoid multiplication by zero
  }

  double rng(double x, double flr = 0.1) {
    double ret = flr;
    if(x<=(best_angle_-low_angle_bnd_)) ret = flr + pow((best_angle_-low_angle_bnd_)-x, 4);
    if(x>= (best_angle_ + high_angle_bnd_)) ret = flr + pow(x- (best_angle_ + high_angle_bnd_), 4);
    return ret;
  }

  double cost(double view_ang_diff, double sun_ang_diff, double gsd_ratio) {
    double c = rng(view_ang_diff);
    c *= sc(sun_ang_diff, max_sun_ang_); // cost for exceeding the max sun angle difference
    c *= sc(log(gsd_ratio), log(max_gsd_ratio_));  // cost for exceeding the max gsd ratio
    return c;                            // product of costs
  }

  // the functor operator to provide a less-than comparison
  bool operator ()(std::pair<size_t, size_t> const& a, std::pair<size_t, size_t> const& b) {

    double view_ang_dif_a = ang_diffs_[a.first][a.second];
    double sun_ang_dif_a = sun_angle_diffs_[a.first][a.second];
    double gsd_rat_a = gsd_ratios_[a.first][a.second];
    double cost_a = cost(view_ang_dif_a, sun_ang_dif_a, gsd_rat_a);

    double view_ang_dif_b = ang_diffs_[b.first][b.second];
    double sun_ang_dif_b = sun_angle_diffs_[b.first][b.second];
    double gsd_rat_b = gsd_ratios_[b.first][b.second];
    double cost_b = cost(view_ang_dif_b, sun_ang_dif_b, gsd_rat_b);
    return cost_a < cost_b;
  }

  double best_angle_;     // the ideal view angle separation for stereo reconstruction
  double low_angle_bnd_;
  double high_angle_bnd_;
  double max_sun_ang_;    // the maximum sun angle difference deemed to not degrade surface quality
  double max_gsd_ratio_;  // the maximum gsd ratio deemed to not degrade surface quality
  std::map<size_t, std::map<size_t, double> > sun_angle_diffs_;
  std::map<size_t, std::map<size_t, double> >& ang_diffs_;
  std::map<size_t, std::map<size_t, double> >& gsd_ratios_;
};

struct bsgm_pair_selector_params
{
  bsgm_pair_selector_params():
    best_angle_(12.0), low_angle_bnd_(2.0), high_angle_bnd_(12.0),
    max_sun_ang_(3.0), max_gsd_ratio_(1.1), max_proj_err_(0.3) {}

  double best_angle_;
  double low_angle_bnd_;
  double high_angle_bnd_;
  double max_sun_ang_;
  double max_gsd_ratio_;
  double max_proj_err_;
};

class bsgm_pair_selector
{
 public:
  bsgm_pair_selector():
    meta_dir_(""), tile_dir_(""), subdir_(""), crop_str_(""),
    affine_cam_postfix_("_affine_corrected.txt"),
    image_meta_fname_("images.json"),
    geo_corr_meta_fname_("geo_corr_metadata.json"),
    non_seed_geo_corr_meta_fname_("geo_corr_non_seed_metadata.json"),
    pair_output_fname_("sorted_image_pairs.json"){}

  //: set and return parameter block
  void set_params(bsgm_pair_selector_params const& params) {params_ = params;}
  bsgm_pair_selector_params params() {return params_;}

  //: add the crop file postfix if needed for loading affine cameras
  void add_crop_postfix(bool add_crop) {add_crop?crop_str_="_crop":crop_str_="";}

  //: load uncorrected affine cams
  bool load_uncorr_affine_cams(std::string affine_cam_path);

  //: the path to the directory containing images and cams with the best correction constraints (seed tile)
  void set_tile_dir(std::string const& dir) {tile_dir_ = dir;}

  //: subdirectory in tile dir to put results
  void set_subdir(std::string const& dir) { subdir_ = dir; }

  //: the path to the directory containing metadata files
  void set_metadata_dir(std::string const& dir) {meta_dir_ = dir;}

  //: set the conventions for file names
  void set_path_conventions(std::string const& affine_cam_postfix, std::string const& image_meta_fname,
                            std::string const& geo_corr_meta_fname, std::string const& pair_output_fname) {
    affine_cam_postfix_ = affine_cam_postfix; image_meta_fname_=image_meta_fname;
    geo_corr_meta_fname_ = geo_corr_meta_fname; pair_output_fname_ = pair_output_fname;}

  //: read the metadata for images, e.g. sun angle, gsd
  bool read_image_metadata();

  //: read the camera translation metadata to be able to eliminate cameras with excessive projection error
  // the option specified if pairs are to be formed only with seed corrected cameras
  bool read_geo_corr_metadata(bool seed_cameras_only);

  //: iterate through the corrected cameras and remove those with to large a projection error
  bool set_inames();

  //: sort the pairs based on image metadata
  bool prioritize_pairs();

  //: the main pair selection process method. default includes both seed and non-seed corrected cameras in the selection
  bool process(bool seed_cameras_only = false) {
    bool good = read_image_metadata();
    good = good && read_geo_corr_metadata(seed_cameras_only);
    good = good && set_inames();
    good = good && prioritize_pairs();
    return good;
  }

  //: accessors to sorted pairs
  std::vector<std::pair<size_t, size_t> > pair_indices() const {return ordered_pairs_;}
  std::vector<std::pair<std::string, std::string> > pair_inames();
  std::vector<bsgm_pair_info> pair_information() {return ordered_pair_info_;}
  //: save a json file of ordered pair information to pairwise process directory
  bool save_ordered_pairs();

  //: save the full pair ordering information to the meta directory
  bool save_all_ordered_pairs();

  // for debug purposes
  void print_ordered_pairs();
  void print_pair_el_az();
  vgl_vector_3d<double>  sun_direction(std::string const& iname);
  double gsd(std::string const& iname);

 private:
  // internal methods
  void cache_pair_info();
  std::string remove_crop_postfix(std::string const& iname);

  // members
  bsgm_pair_selector_params params_;
  acal_metadata meta_;
  acal_metadata non_seed_meta_;

  std::string meta_dir_;
  std::string tile_dir_;
  std::string subdir_;
  std::string crop_str_;
  std::string affine_cam_postfix_;
  std::string image_meta_fname_;
  std::string geo_corr_meta_fname_;
  std::string non_seed_geo_corr_meta_fname_;
  std::string pair_output_fname_;
  std::vector<std::pair<size_t, size_t> > ordered_pairs_;
  std::vector<bsgm_pair_info> ordered_pair_info_;
  std::map<size_t, std::string > inames_;
  std::map<size_t, vpgl_affine_camera<double> > acams_;
  std::vector<double> gsds_;
  std::map<size_t, dtime> dtimes_;
  std::map<size_t, std::map<size_t, int> > time_diffs_;
  std::map<size_t, std::map<size_t, double> > ang_diffs_;
  std::map<size_t, std::map<size_t, std::pair<double, double> > > view_az_el_;
  std::map<size_t, std::map<size_t, double> > sun_angle_diffs_;
  std::map<size_t, std::map<size_t, double> > gsd_ratios_;
  std::map<size_t, std::map<size_t, bool> > no_ray_degen_;
};
#endif
