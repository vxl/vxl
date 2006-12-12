#ifndef rgrl_feature_set_bins_h_
#define rgrl_feature_set_bins_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003
//
// \verbatim
//  Modifications
//   Chuck Stewart - 8 Nov 2005 - added versions of nearest_feature and k_nearest_feature 
//      based on point location alone 
// \endverbatim

#include <rsdl/rsdl_fwd.h>
#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_mask.h>

#include <vcl_memory.h>

//: A set of features grouped only by N-d location.
//
// This class is used to store a collection of discrete features for
// which proximity is determined only by location. There is no
// concepts of "segments"; each feature is considered independent.
//
// The class is templated over the dimension in order to provide
// efficient implementation of the queries.
//
// The underlying data structure is either rdsl_bins or
// rsdl_kd_tree. when using rsdl_bins, the image region is divided
// into rectangular bins, with the bin_size defaulted to 10 or defined
// by the user during construction. Feature point locations and
// associated values are stored in a list associated with the bin the
// location falls into. When performing region searches, all bins that
// intersect the query region are examined. When doing
// nearest-neighbor searches, spiral search is performed, starting at
// the bin containing the query point. rsdl_bins is the choice if the
// data is well distributed.
//
// rsdl_kd_tree is multi-dimensional version of binary search tree. It
// handles data clustering better than rsdl_bins. Example applicaitons
// include range data registration.
//
template<unsigned N>
class rgrl_feature_set_bins
  : public rgrl_feature_set
{
 public:
  //: Constructor.
  //  \a bin_size is only effective when \a use_bins is set true.
  rgrl_feature_set_bins( feature_vector const& features,
                         double bin_size = 10,
                         rgrl_feature_set_label const& label = rgrl_feature_set_label() );

  ~rgrl_feature_set_bins();

  void
  features_in_region( feature_vector& results, rgrl_mask_box const& roi ) const;

  //:  Return the features in a given circle/sphere.
  //
  void
  features_within_radius( feature_vector& results, vnl_vector<double> const& center, double radius ) const;

  //: Nearest feature based on Euclidean distance
  rgrl_feature_sptr
  nearest_feature( const vnl_vector<double>& loc ) const;

  //: Nearest feature based on Euclidean distance
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr const& feature ) const;

  //: Return all features within a given Euclidean distance
  void
  features_within_distance( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const;

  //:  Return the k nearest features based on Euclidean distance.
  void
  k_nearest_features( feature_vector& results, const vnl_vector<double>& feature_loc, unsigned int k ) const;

  //:  Return the k nearest features based on Euclidean distance.
  void
  k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const;

  //:  Return the bounding box encloses the feature set
  rgrl_mask_box
  bounding_box() const;

  //:  Return the type of feature
  const vcl_type_info&
  type() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set_bins, rgrl_feature_set );

 private:
  typedef rsdl_bins<N,double,rgrl_feature_sptr> bin_type;

  const vcl_type_info* feature_type_;

  rgrl_mask_box bounding_box_;

  // Using bins as the data structure
  vcl_auto_ptr< bin_type > bins_;

  // Using kd_tree as the data structure
  //feature_vector features_;
  //rsdl_kd_tree_sptr kd_tree_;
};


#endif // rgrl_feature_set_bins_h_
