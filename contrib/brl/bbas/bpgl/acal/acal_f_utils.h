// This is//external/acal/acal_f_utils.h
#ifndef acal_f_utils_h
#define acal_f_utils_h

//:
// \file
// \brief A class to work with geo correction f_utils
// \author J.L. Mundy
// \date Oct 16, 2018
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vil/vil_image_view.h>

#include "acal_match_utils.h"

// parameters for filtering based on epipolar line distance
struct f_params
{
  f_params():epi_dist_mul_(2.5), max_epi_dist_(5.0), F_similar_abcd_tol_(0.01), F_similar_e_tol_(1.0), ray_uncertainty_tol_(50.0), min_num_matches_(5){}
  double epi_dist_mul_; //a multiplier on the error for the lowest 10% of epipolar line distances
  double max_epi_dist_; //an absolute threshold on epipolar line distances in case error estimation fails
  double F_similar_abcd_tol_;  //max abs difference |a+c| + |b+d|, a measure of viewpoint similarity
  double F_similar_e_tol_;     //max abs value of offset, e to determine similar images
  double ray_uncertainty_tol_; //max ray uncertainty to keep camera pair
  size_t min_num_matches_;     //minimum number of required matches to output to fmatches file

  bool operator==(f_params const& other) const {
    return this->epi_dist_mul_ == other.epi_dist_mul_ &&
           this->max_epi_dist_ == other.max_epi_dist_ &&
           this->F_similar_abcd_tol_ == other.F_similar_abcd_tol_ &&
           this->F_similar_e_tol_ == other.F_similar_e_tol_ &&
           this->ray_uncertainty_tol_ == other.ray_uncertainty_tol_ &&
           this->min_num_matches_ == other.min_num_matches_;
  }
  bool operator!=(f_params const& other) const {
    return !(*this == other);
  }

};

struct dtime
{
  dtime() {}
  dtime(unsigned year, unsigned month, unsigned day, unsigned hours, unsigned minutes, unsigned seconds):
  year_(year), month_(month), day_(day), hours_(hours), minutes_(minutes), seconds_(seconds){}

  //: compute t1 - t0 in seconds
  static int time_diff(dtime const& t0, dtime const& t1);

  //: order on time in seconds
  static bool time_less(dtime const& ta, dtime const& tb);

  unsigned year_;
  unsigned month_;
  unsigned day_;
  unsigned hours_;
  unsigned minutes_;
  unsigned seconds_;
};

class acal_f_utils
{
 public:

  //:load a local rational camera
  static vpgl_camera_double_sptr load_camera(std::string const& cam_path);

  //: write fmatches file (visual sfm standard format)
  static bool write_f_matches(
      std::string fmatches_path,
      std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > > const& matches,
      std::map<size_t, std::string> & image_paths);

  //: read fmatches file (visual sfm standard format)
  static bool read_f_matches(
      std::string fmatches_path,
      std::map<size_t, std::map<size_t, std::vector<acal_match_pair> > >& matches,
      std::map<size_t, std::string>& image_paths);

  //: io for map of affine cameras produced by sift_features
  static bool write_affine_cameras(
      std::map<size_t, vpgl_affine_camera<double> >& acams,
      std::string affine_cam_path);

  static bool read_affine_cameras(
      std::string affine_cam_path,
      std::map<size_t, vpgl_affine_camera<double> >& acams);

  //: reject matches with too large a Euclidean distance from
  //  the epipolar line in image2 defined by the correspondence in image1
  //  F is the fundamental matrix for the image pair 1,2
  static size_t filter_matches(
      vnl_matrix_fixed<double, 3, 3> const& F,
      std::vector<acal_match_pair>& matches);

  //: the determinant of linear ray intersection coeficient matrix,
  //  r1, r2 affine camera ray directions (unit vectors)
  //
  //          |r1.r1  -r1.r2|
  //   det  = |             | = 1 - (r1.r2)^2 , uncertainty = 1/det;
  //          |-r1.r2  r2.r2|
  //
  static double ray_uncertainty(
      vpgl_affine_camera<double> const& acam1,
      vpgl_affine_camera<double> const& acam2);

  //: find 3-d intersection points of tracks
  // use map as output in case some intersections fail - the map key is input track vector index
  static bool intersect_tracks_with_3d(
      std::map<size_t, vpgl_affine_camera<double> >& cams,
      // tracks             cam_id  corr pt
      std::vector< std::map<size_t, vgl_point_2d<double> > > const& tracks,
      std::map<size_t, vgl_point_3d<double> >& inter_pts,
      std::map<size_t, std::map<size_t, vgl_point_2d<double> > > & projected_tracks,
      vnl_matrix<double> const& ray_covariance = vnl_matrix<double>(0,0),
      size_t cam_mask=-1);

  // different signature for convenience
  // use map as output in case some intersections fail - the map key is input track vector index
  static bool intersect_tracks(
      std::map<size_t, vpgl_affine_camera<double> > & cams,
      // tracks             cam_id  corr pt
      std::vector< std::map<size_t, vgl_point_2d<double> > > const& tracks,
      std::map<size_t, std::map<size_t, vgl_point_2d<double> > >& projected_tracks,
      size_t cam_mask = -1,
      bool verbose = false);

  static bool datetime_from_iname(std::string const& iname, dtime& datetime);

 private:
  acal_f_utils();

};

#endif
