#ifndef rgrl_feature_set_h_
#define rgrl_feature_set_h_
//:
// \file
// \brief Base class to represent and provide access in several ways to a set of registration features.
// \author Chuck Stewart
// \date 12 Nov 2002

#include <vcl_vector.h>
#include <vcl_string.h>

#include "rgrl_feature.h"
#include "rgrl_object.h"

class rgrl_feature_set_label;
class rgrl_mask_box;

#include "rgrl_feature_set_sptr.h"

//: Base class to represent and provide access in several ways to a set of registration features.
//
//  Each feature set represents features of one type and at one resolution.
//
class rgrl_feature_set
  : public rgrl_object
{
 public:
  typedef vcl_vector<rgrl_feature_sptr>  feature_vector;

 public:
  rgrl_feature_set();

  virtual ~rgrl_feature_set();

  //:  Return the features in a given ROI.
  //
  virtual
  feature_vector
  features_in_region( rgrl_mask_box const& roi ) const = 0;

  //: Return the nearest feature to the given feature.
  //
  // This may be constructed from an underlying continuous
  // representation.
  //
  virtual
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr feature ) const = 0;

  //:  Return all features within a given distance, one per segment (if segmented)
  //
  virtual
  feature_vector
  features_within_distance( rgrl_feature_sptr feature, double distance ) const = 0;

  //:  Return the k nearest features, one per segment (if segmented)
  //
  virtual
  feature_vector
  k_nearest_features( rgrl_feature_sptr feature, unsigned int k ) const = 0;

  //:  Return the bounding box encloses the feature set
  virtual
  rgrl_mask_box
  bounding_box() const = 0;
  //:  Return the type of feature
  //
  virtual
  const vcl_type_info&
  type() const = 0;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_set, rgrl_object );

 private:
  //disabled
  rgrl_feature_set( rgrl_feature_set const& other );
  rgrl_feature_set& operator=( rgrl_feature_set const& other );
};


//: User-friendly label for the feature sets.
//
// The labels are used to refer to the feature sets in the context of
// an rgrl_data_set.
//
// \sa rgrl_data_set
//
class rgrl_feature_set_label
{
 public:
  //:
  rgrl_feature_set_label( const vcl_string& name );

  //:
  bool operator==( rgrl_feature_set_label const& other ) const;

  //:
  bool operator!=( rgrl_feature_set_label const& other ) const;

  //:
  bool operator<( rgrl_feature_set_label const& other ) const;

 private:
  vcl_string name_;
};


#endif
