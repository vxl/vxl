// This is brl/bseg/bapl/bapl_lowe_cluster.cxx
//:
// \file

#include <cmath>
#include <iostream>
#include <algorithm>
#include "bapl_lowe_cluster.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_affine2d_est.h>

#include <rrel/rrel_ran_sam_search.h>
#include <rrel/rrel_muset_obj.h>


//: Constructor
bapl_lowe_clusterer::bapl_lowe_clusterer( int max_image_dim1,
                                          int max_image_dim2,
                                          double max_scale )
{
  max_scale_ = int(std::log(max_scale)/std::log(2.0))+1;
  trans_step_ = max_image_dim1/4;
  max_trans_ = (max_image_dim1+max_image_dim2)/trans_step_;

  hash_.resize(8*max_scale_*max_trans_*max_trans_*12);
}


//: Hash the keypoint match into the table
void
bapl_lowe_clusterer::hash(const bapl_keypoint_match& match)
{
  all_matches_.push_back(match);

  double d_trans_i = match.second->location_i() - match.first->location_i();
  double d_trans_j = match.second->location_j() - match.first->location_j();
  double d_scale = match.second->scale()/match.first->scale();
  double d_orient = match.second->orientation() - match.first->orientation();

  int i_bin = int(d_trans_i/trans_step_) + max_trans_;
  if ( i_bin < 0 ) i_bin = 0;
  if ( i_bin > 2*max_trans_-1 ) i_bin = 2*max_trans_-1;

  int j_bin = int(d_trans_j/trans_step_) + max_trans_;
  if ( j_bin < 0 ) j_bin = 0;
  if ( j_bin > 2*max_trans_-1 ) j_bin = 2*max_trans_-1;

  int s_bin = int(std::log(d_scale)/std::log(2.0)) + max_scale_;
  if ( s_bin < 0 ) s_bin = 0;
  if ( s_bin > 2*max_scale_-1 ) s_bin = 2*max_scale_-1;

  int o_bin = int(d_orient*12.0f/6.28319f)%12;

  for (int i=i_bin; i<=i_bin+1; ++i)
    for (int j=j_bin; j<=j_bin+1; ++j)
      for (int s=s_bin; s<=s_bin+1; ++s)
        for (int o=o_bin; o<=o_bin+1; ++o){
          int index = (((((i*2*max_trans_)+j)*2*max_scale_)+s)*12)+(o%12);
          hash_[index].push_back(match);
        }
}

bool sort_by_size( const std::vector< bapl_keypoint_match > & lhs,
                   const std::vector< bapl_keypoint_match > & rhs )
{
  return lhs.size() > rhs.size();
}

//: Fill \param clusters with the clusters sorted by cluster size
std::vector< std::vector< bapl_keypoint_match > >
bapl_lowe_clusterer::get_sorted_clusters() const
{
  std::vector< std::vector< bapl_keypoint_match > > clusters(hash_);
  std::sort(clusters.begin(), clusters.end(), sort_by_size);
  int last_valid = -1;
  while ( !clusters[++last_valid].empty() );
  clusters.resize(last_valid);
  return clusters;
}


//: Estimate a 2D affine transform from the clusters
bool
bapl_lowe_clusterer::estimate_all_affine( std::vector< bapl_affine_transform >& transforms,
                                          std::vector< bapl_keypoint_match >& inliers ) const
{
  std::vector< std::vector< bapl_keypoint_match > > clusters = this->get_sorted_clusters();

  transforms.clear();
  inliers.clear();
  vnl_vector<double> p(6, 0.0);
  p[0] = p[3] = 1.0;

  for (unsigned c=0; c<clusters.size() && clusters[c].size()>2; ++c){
    std::cout << "testing cluster "<<c<<std::endl;

    bapl_affine_transform A;
    std::vector< bapl_keypoint_match >& matches = clusters[c];

    // Get an initial estimate and remove outliers
    if ( !this->estimate_affine(A, matches) ) continue;

    // Add all matches that agree with the estimate
    matches.clear();
    for (const auto & all_matche : all_matches_) {
      if ( this->is_inlier(A, all_matche) )
        matches.push_back(all_matche);
    }

    // Get a better estimate and remove outliers
    if ( !this->estimate_affine(A, matches) ) continue;

    transforms.push_back(A);
    // for now, merge matches into inliers
    for (auto & matche : matches) {
      bool already_found = false;
      for (auto & inlier : inliers) {
        if ( matche.second == inlier.second ){
          already_found = true;
          break;
        }
      }
      if ( !already_found ) inliers.push_back(matche);
    }
    std::cout << "current inliers size: "<<matches.size() << std::endl;
  }
  std::cout << "estimated "<<transforms.size()<<" transforms"<<std::endl;
  if (transforms.empty())
    inliers = all_matches_;

  return true;
}


//: Estimate a 2D affine transform from the matches
bool
bapl_lowe_clusterer::estimate_affine( bapl_affine_transform& transform,
                                      std::vector< bapl_keypoint_match >& matches ) const
{
  unsigned num_inliers = 0;
  while ( num_inliers!=matches.size() ) {
    num_inliers = matches.size();

    //std::cout << " # of inliers = " << inliers.size() << std::endl;
    bapl_affine2d_est est( matches );

    vnl_vector<double> p;
    vnl_matrix<double> covar;
    if ( !est.weighted_least_squares_fit(p,covar) ){
      return false;
    }
    else {
      transform = bapl_affine_transform(p[0], p[1], p[2], p[3], p[4], p[5]);

      for ( auto itr = matches.begin();
            itr != matches.end();  ++itr ) {
        if ( !this->is_inlier(transform, *itr) ){
          matches.erase(itr);
          --itr;
        }
      }
      if ( matches.size()<3 ){
        return false;
      }
    }
  }
  return true;
}


//: Determine if a match is an inlier given an affine transformation;
bool
bapl_lowe_clusterer::is_inlier( const bapl_affine_transform& A,
                                const bapl_keypoint_match& match ) const
{
  double x_obj = match.first->location_i();
  double y_obj = match.first->location_j();
  double o_obj = match.first->orientation();
  double s_obj = match.first->scale();
  double x2_obj = x_obj + s_obj*(std::cos(o_obj));
  double y2_obj = y_obj + s_obj*(std::sin(o_obj));

  double x_tgt = match.second->location_i();
  double y_tgt = match.second->location_j();
  double o_tgt = match.second->orientation();
  double s_tgt = match.second->scale();

  double x_map, y_map, x2_map, y2_map;
  A.map(x_obj,y_obj,x_map,y_map);
  A.map(x2_obj,y2_obj,x2_map,y2_map);

  double dx = x2_map - x_map;
  double dy = y2_map - y_map;
  double s_map = std::sqrt(dx*dx + dy*dy);
  double o_map = std::atan2(dy,dx);

  double err_x = x_tgt - x_map;
  double err_y = y_tgt - y_map;
  double err_s = std::log(s_tgt/s_map)*vnl_math::log2e; // i.e., the base-2 log of s_tgt/s_map
  double err_o = vnl_math::angle_minuspi_to_pi(o_tgt-o_map);

  return std::fabs(err_o) < 0.25 &&
         std::fabs(err_s) < 0.5 &&
         std::fabs(err_x) < trans_step_*0.25 &&
         std::fabs(err_y) < trans_step_*0.25;
}
