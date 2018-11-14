#ifndef rgrl_feature_landmark_h_
#define rgrl_feature_landmark_h_
//:
// \file
// \brief Derived class to represent a feature that is a landmark point - a branching or cross-over of vascular.
// \author Chuck Stewart
// \date 12 Nov 2002
// \verbatim
// Modifications
//      Nov 2008 J Becker: Added a clone function.
// \endverbatim

#include <vector>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vbl/vbl_array_2d.h>

#include "rgrl_feature.h"

//: Represents a landmark feature.
//
// A landmark feature is a branching or cross-over point of a
// vascular-like structure. It is characterized by a number of outgoing
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
                         std::vector< vnl_vector<double> >  outgoing_directions );

  //: read in feature

  bool read( std::istream& is, bool skip_tag=false ) override;

  //: write out feature

  void write( std::ostream& os ) const override;

  vnl_matrix<double> const&
  error_projector() const override;

  vnl_matrix<double> const&
  error_projector_sqrt() const override;

  unsigned int
  num_constraints() const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_landmark, rgrl_feature );

  rgrl_feature_sptr transform( rgrl_transformation const&  xform ) const override;

  //:  signature_weight from the signature similarity
  //
  double absolute_signature_weight( rgrl_feature_sptr other ) const override;

  //: make a clone copy
  rgrl_feature_sptr clone() const override;

 protected:
  friend class rgrl_feature_reader;
  rgrl_feature_landmark( rgrl_feature_landmark const& other );

  //: uninitialized constructor
  rgrl_feature_landmark();

  // to be able to use the protected constructor
  friend rgrl_feature_sptr
         rgrl_feature_reader( std::istream& is );

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
  double max_similarity(const std::vector<vnl_vector<double> >& u,
                        const std::vector<vnl_vector<double> >& v,
                        int count,
                        const vbl_array_2d<bool>& invalid) const;

 private:
  // For the center location of the landmark
  vnl_matrix<double> error_proj_;

  // For the signature of the landmark
  std::vector< vnl_vector<double> >  outgoing_directions_;
};

#endif // rgrl_feature_landmark_h_
