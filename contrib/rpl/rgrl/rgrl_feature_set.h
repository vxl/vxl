#ifndef rgrl_feature_set_h_
#define rgrl_feature_set_h_
//:
// \file
// \brief Base class to represent and provide access in several ways to a set of registration features.
// \author Chuck Stewart
// \date 12 Nov 2002
//
// \verbatim
//  Modifications
//   Chuck Stewart - 8 Nov 2005 - added versions of nearest_feature and k_nearest_feature 
//      based on point location alone 
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>

#include "rgrl_feature.h"
#include "rgrl_object.h"

class rgrl_feature_set_label;
class rgrl_mask_box;

#include "rgrl_feature_set_sptr.h"

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
  rgrl_feature_set_label( const vcl_string& name = vcl_string() );

  //: return name
  vcl_string const& name() const
  { return name_; }
  
  //:
  bool operator==( rgrl_feature_set_label const& other ) const;

  //:
  bool operator!=( rgrl_feature_set_label const& other ) const;

  //:
  bool operator<( rgrl_feature_set_label const& other ) const;

 private:
  vcl_string name_;
};

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
  rgrl_feature_set( feature_vector const& fea_vec, rgrl_feature_set_label const& label = rgrl_feature_set_label() );

  virtual ~rgrl_feature_set();

  //: set label
  void set_label( rgrl_feature_set_label const& label )
  { label_ = label; }
  
  //: get label
  rgrl_feature_set_label const& label() const
  { return label_; }
  
  //:  Return all the features
  //
  virtual
  feature_vector const&
  all_features( ) const
  { return fea_vec_; }

  //:  Return the features in a given ROI.
  //
  virtual
  void
  features_in_region( feature_vector& results, rgrl_mask_box const& roi ) const = 0;

  //:  Return the features in a given circle/sphere.
  //
  virtual
  void
  features_within_radius( feature_vector& results, vnl_vector<double> const& center, double radius ) const = 0;

  //: Return the nearest feature to the given location.
  //
  // This may be constructed from an underlying continuous
  // representation.
  //
  virtual
  rgrl_feature_sptr
  nearest_feature( const vnl_vector<double>& loc ) const = 0;

  //: Return the nearest feature to the given location.
  virtual
  rgrl_feature_sptr
  nearest_feature( rgrl_feature_sptr const& feature ) const = 0;

  //:  Return all features within a given distance of the given feature, one per segment (if segmented)
  //
  virtual
  void
  features_within_distance( feature_vector& results, rgrl_feature_sptr const& feature, double distance ) const = 0;

  //:  Return the k nearest features to the given location, one per segment (if segmented)
  //
  virtual
  void
  k_nearest_features( feature_vector& results, const vnl_vector<double>& feature_loc, unsigned int k ) const = 0;

  //:  Return the k nearest features, one per segment (if segmented)
  //
  virtual
  void
  k_nearest_features( feature_vector& results, rgrl_feature_sptr const& feature, unsigned int k ) const = 0;

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

 protected:
  rgrl_feature_set_label label_;
  feature_vector         fea_vec_;
};



#endif
