#ifndef rgrl_feature_point_h_
#define rgrl_feature_point_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_feature.h"

//: Represent a point in space.
//
// This type of feature can be used to represent simple point-to-point
// correspondences.
//
class rgrl_feature_point
  : public rgrl_feature
{
 public:
  //:
  rgrl_feature_point( vnl_vector<double> const& loc );

  vnl_vector<double> const&
  location() const;

  //: Provide the scale level at which this feature is detected
  virtual 
  double scale() const { return scale_; }
  
  //: Set the scale level at which this feature is detected
  virtual 
  void set_scale( double scale ) { scale_ = scale; }

  vnl_matrix<double> const&
  error_projector() const;

  //: Result is a rgrl_feature_point.
  virtual
  rgrl_feature_sptr
  transform( rgrl_transformation const& xform ) const;

  unsigned int
  num_constraints() const;

  //:  Compute the signature weight between two features.
  virtual double absolute_signature_weight( rgrl_feature_sptr other ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_point, rgrl_feature );

 protected:
  //:
  // Create an uninitialized feature with enough space to store a dim
  // dimensional feature.
  //
  rgrl_feature_point( unsigned dim );

  vnl_vector<double> location_;
  
  double             scale_;
};

#endif // rgrl_feature_point_h_
