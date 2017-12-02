#ifndef rgrl_feature_point_h_
#define rgrl_feature_point_h_
//:
// \file
// \author Amitha Perera
// \date   Feb 2003
// \verbatim
// Modifications
//      Nov 2008 J Becker: Added a clone function.
// \endverbatim

#include <iostream>
#include <iosfwd>
#include "rgrl_feature.h"
#include <vcl_compiler.h>

//: Represent a point in space.
//
// This type of feature can be used to represent simple point-to-point
// correspondences.
//
class rgrl_feature_point
  : public rgrl_feature
{
 public:

  //:ctor
  rgrl_feature_point( vnl_vector<double> const& loc );

  //:ctor
  rgrl_feature_point( vnl_vector<double> const& loc, double scale );

  //: read in feature
  virtual
  bool read( std::istream& is, bool skip_tag=false );

  //: write out feature
  virtual
  void write( std::ostream& os ) const;

  vnl_matrix<double> const&
  error_projector() const;

  vnl_matrix<double> const&
  error_projector_sqrt() const;

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

  //: make a clone copy
  virtual rgrl_feature_sptr clone() const;

 protected:
  friend class rgrl_feature_reader;
  //:
  // Create an uninitialized feature with enough space to store a dim
  // dimensional feature.
  //
  rgrl_feature_point( unsigned dim=0 );

  // to be able to use the protected constructor
  friend rgrl_feature_sptr
         rgrl_feature_reader( std::istream& is );

  //: Apply transformation to the scale property
  virtual double
  transform_scale( rgrl_transformation const& xform ) const;

 protected:
  mutable vnl_matrix<double> err_proj_, err_proj_sqrt_;
};

#endif // rgrl_feature_point_h_
