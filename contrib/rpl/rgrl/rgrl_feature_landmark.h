
#ifndef rgrl_feature_landmark_h_
#define rgrl_feature_landmark_h_

// \file
// \brief Derived class to represent a feature that is a landmark
// point - a branching or cross-over of vascular.
// \author Chuck Stewart
// \date 12 Nov 2002

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vbl/vbl_array_2d.h>

#include "rgrl_feature.h"

//: Represents a landmark feature.
//
// A landmark feature is a branching or cross-over point of a
// vascular-like strucure. It is characterized by a number of outgoing
// direction vectors. The error_projection matrix is identity.
//
class rgrl_feature_landmark
  : public rgrl_feature
{
public:  
  //:  Constructor to initialize feature_landmark location.
  //
  // Works for 2d and 3d.
  rgrl_feature_landmark( vnl_vector<double> const& loc,
			 vcl_vector< vnl_vector<double> > const& outgoing_directions );

  vnl_vector<double> const&
  location() const;
  
  vnl_matrix<double> const&
  error_projector() const;

  unsigned int
  num_constraints() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_landmark, rgrl_feature );

  rgrl_feature_sptr transform( rgrl_transformation const&  xform ) const;

  //:  signature_weight from the signature similarity
  //
  double absolute_signature_weight( rgrl_feature_sptr other ) const;

private:
  rgrl_feature_landmark( rgrl_feature_landmark const& other );

  // disabled, not implemented
  rgrl_feature_landmark& operator=( rgrl_feature_landmark const& );
  
  //:  Compute the maximum similarity between 2 landmarks.
  //
  //   The similarity is a measure of best alignment of the 2 sets of
  //   outgoing directions. It is computed as the sum of the
  //   dot-product of the matched directions, normalized by twice the
  //   size of the smallest set of outgoing directions. The details
  //   are in "A Feature-Based, Robust, Hierarchical Algorithm for
  //   Registering Pairs of Images of the Curved Human Retina", 
  //   IEEE Pattern Analysis and Machine Intelligence, 24(3), 2002. 
  //
  double max_similarity(const vcl_vector<vnl_vector<double> >& u,
                        const vcl_vector<vnl_vector<double> >& v,
                        int count, 
                        const vbl_array_2d<bool>& invalid) const;

private:
  // For the center location of the landmark
  vnl_vector<double> location_;
  vnl_matrix<double> error_proj_;

  // For the signature of the landmark
  vcl_vector< vnl_vector<double> >  outgoing_directions_;
};

#endif // rgrl_feature_landmark_h_
