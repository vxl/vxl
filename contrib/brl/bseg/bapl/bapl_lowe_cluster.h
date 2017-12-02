// This is brl/bseg/bapl/bapl_lowe_cluster.h
#ifndef bapl_lowe_cluster_h_
#define bapl_lowe_cluster_h_
//:
// \file
// \brief A generalized Hough transform to cluster keypoint matches
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sun Nov 16 2003
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <iostream>
#include <utility>
#include <vcl_compiler.h>
#include <bapl/bapl_lowe_keypoint_sptr.h>
#include <bapl/bapl_affine_transform.h>

typedef std::pair< bapl_lowe_keypoint_sptr, bapl_lowe_keypoint_sptr > bapl_keypoint_match;

//: This class clusters matches into bins
// Hashing is according to the similarity transformation parameters
// defined between the two keypoints.
class bapl_lowe_clusterer
{
 public:
  //: Constructor
  bapl_lowe_clusterer(int max_image_dim1, int max_image_dim2, double max_scale);
  //: Destructor
  ~bapl_lowe_clusterer(){}

  //: Hash the keypoint match into the table
  void hash(const bapl_keypoint_match& match);

  //: Return the number of buckets
  int size() const { return hash_.size(); }

  //: Return the cluster of matches in bucket b
  const std::vector< bapl_keypoint_match >& cluster_at(int b) const { return hash_[b]; }

  //: Fill \param clusters with the clusters sorted by cluster size
  std::vector< std::vector< bapl_keypoint_match > > get_sorted_clusters() const;

  //: Estimate a 2D affine transform from the clusters
  bool estimate_all_affine( std::vector< bapl_affine_transform >& transforms,
                            std::vector< bapl_keypoint_match >& inliers ) const;

 private:
  //: Estimate a 2D affine transform from the matches
  // The estimate is returned in transform and the inliers are returned in matches
  bool estimate_affine( bapl_affine_transform& transform,
                        std::vector< bapl_keypoint_match >& matches ) const;

  //: Determine if a match is an inlier given an affine transformation;
  bool is_inlier( const bapl_affine_transform& A, const bapl_keypoint_match& match ) const;

  //: Table for hashing matches based on parameters
  std::vector< std::vector< bapl_keypoint_match > > hash_;

  //: A vector of all matches in the hash table
  std::vector< bapl_keypoint_match > all_matches_;

  int max_trans_;
  int trans_step_;
  int max_scale_;
};

#endif // bapl_lowe_cluster_h_
