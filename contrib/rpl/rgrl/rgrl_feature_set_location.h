#ifndef rgrl_feature_set_location_h_
#define rgrl_feature_set_location_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <rsdl/rsdl_fwd.h>

#include "rgrl_feature.h"
#include "rgrl_feature_set.h"
#include "rgrl_mask.h"

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
class rgrl_feature_set_location
  : public rgrl_feature_set
{
 public:
  //: Constructor.
  //  \a bin_size is only effective when \a use_bins is set true.
  rgrl_feature_set_location( feature_vector const& features,
                             bool use_bins = true,
                             double bin_size = 10 );

  ~rgrl_feature_set_location();

  feature_vector
  features_in_region( rgrl_mask_box const& roi ) const;

  //: Nearest feature based on Euclidean distance
  //
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr feature ) const;

  //: Return all features within a given Euclidean distance
  //
  feature_vector
  features_within_distance( rgrl_feature_sptr feature, double distance ) const;

  //:  Return the k nearest features based on Euclidean distance.
  feature_vector
  k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const;

  //:  Return the bounding box encloses the feature set
  rgrl_mask_box
  bounding_box() const;

  //:  Return the type of feature
  const vcl_type_info&
  type() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set_location, rgrl_feature_set );

 private:
  typedef rsdl_bins<N,double,rgrl_feature_sptr> bin_type;

  const vcl_type_info* feature_type_;

  rgrl_mask_box bounding_box_;

  // Using bins as the data structure
  vcl_auto_ptr< bin_type > bins_;
  bool use_bins_;

  // Using kd_tree as the data structure
  feature_vector features_;
  rsdl_kd_tree_sptr kd_tree_;
};


#endif // rgrl_feature_set_location_h_
