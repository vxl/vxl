// This is//external/acal/acal_match_utils.h
#ifndef acal_match_utils_h
#define acal_match_utils_h

//:
// \file
// \brief A class to support correspondence computations
// \author J.L. Mundy
// \date January 13, 2019
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

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vil/vil_image_view.h>


// a simple feature structure, just id and location
struct acal_corr{
	acal_corr() : id_(-1) { pt_.set(-1, -1); }
  acal_corr(size_t id, vgl_point_2d<double> const& pt):id_(id),pt_(pt) {}
  size_t id_;
  vgl_point_2d<double> pt_; // correspondence point
};


// a structure to hold information regarding correspondence matches
struct acal_match_pair{
acal_match_pair(){}
acal_match_pair(acal_corr const& corr1, acal_corr const& corr2):
  corr1_(corr1), corr2_(corr2){}
  acal_corr corr1_;// sift feature id and position in image 1
  acal_corr corr2_;// sift feature id and position in image 2
  static bool near_equal(acal_match_pair const& mpa, acal_match_pair const& mpb, double tol = 0.1){
    vgl_vector_2d<double> dif1 = mpa.corr1_.pt_-mpb.corr1_.pt_;
    vgl_vector_2d<double> dif2 = mpa.corr2_.pt_ -mpb.corr2_.pt_;
    double dist1 = dif1.length();
    double dist2 = dif2.length();
    return ((dist1 < tol) && (dist2 < tol));
  }
};


class acal_match_utils
{
 public:
  acal_match_utils(){}

  //: intersect two match pair sets and return the intersected results
  // the intersect_a1_with_b1 flag indicates if corr1_a and corr1_b are matched or corr1_a is matched with corr_2_b
  // normally the order of mpairs_a and mpairs_b should be the same
  static void intersect_match_pairs(
      std::vector<acal_match_pair> const& mpairs_a,
      std::vector<acal_match_pair> const& mpairs_b,
      std::vector<acal_match_pair>& intersected_mpairs_a,
      std::vector<acal_match_pair>& intersected_mpairs_b,
      bool intersect_a1_with_b1=true);

  //: corr_2_a is matched with corr_1b, the correct correspondence order is assumed
  static void reduce_match_pairs_a_by_b(
      std::vector<acal_match_pair> const& mpairs_a,
      std::vector<acal_match_pair> const& mpairs_b,
      std::vector<acal_match_pair>& reduced_mpairs_a,
      std::vector<acal_match_pair>& reduced_mpairs_b);

  //: redude a set of match pairs so that reduced_node_mpairs has its corr1_s the same as reduced_node_corrs
  static void reduce_node_mpairs(
      std::vector<acal_match_pair> const& node_mpairs,
      std::vector<acal_corr> const& reduced_node_corrs,
      std::vector<acal_match_pair> & reduced_node_mpairs);

  //: reverse the set of match pairs so corr1_ -> corr2_ and corr2_->corr1_
  static void reverse(std::vector<acal_match_pair>& mpairs);

  // debug purposes
  //: display the match incidence matrix
  static bool write_match_matrix(
      std::string match_matrix_path, std::map<size_t,
      std::map<size_t, std::vector<acal_match_pair> > > & matches,
      std::map<size_t, std::string>& img_paths);

  //: normalized cross-correlation between two image patches
  static bool norm_xcorr_score(
      vil_image_view<vxl_byte> const& a,
      vil_image_view<vxl_byte> const& b,
      double& score);

  //: normalized cross-correlation of gradient magnitude
  static bool gradient_mag_xcorr_score(
      vil_image_view<vxl_byte> const& a,
      vil_image_view<vxl_byte> const& b,
      double& score);

  //: score based on gradient direction
  static bool gradient_direction_score(
      vil_image_view<vxl_byte> const& a,
      vil_image_view<vxl_byte> const& b,
      double& score);

};

#endif
