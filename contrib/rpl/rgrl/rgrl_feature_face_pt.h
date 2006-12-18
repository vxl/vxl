#ifndef rgrl_feature_face_pt_h_
#define rgrl_feature_face_pt_h_
//:
// \file
// \brief Class to represent a N-d face edge element.
//    It has 1 normal direction and N-1 tangent directions.
//    There could be a defined image region surrounding these.
// \author Chuck Stewart
// \date 16 Sep 2003
// \verbatim
// Modifications
// 17 Oct 2006  GY:  put in signature error vector
// \endverbatim

#include <rgrl/rgrl_feature.h>
#include <vcl_iosfwd.h>

class rgrl_feature_face_pt
  : public rgrl_feature
{
 public:
  rgrl_feature_face_pt( vnl_vector< double > const& location,
                        vnl_vector< double > const& normal );

  //: read in feature
  virtual
  bool read( vcl_istream& is, bool skip_tag=false );

  //: write out feature
  virtual
  void write( vcl_ostream& os ) const;

  virtual 
  vnl_matrix<double> const& 
  error_projector() const;

  virtual 
  vnl_matrix<double> const& 
  error_projector_sqrt() const;

  virtual unsigned int num_constraints() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature_face_pt, rgrl_feature );

  virtual vnl_vector<double> const& normal() const;

  //: Return a matrix whose columns form the subspace tangent to the face normal
  virtual vnl_matrix<double> const&
  tangent_subspace();

  //: Result is a rgrl_feature_face_pt
  virtual rgrl_feature_sptr transform( rgrl_transformation const& xform ) const;

  //:  Compute the signature weight between two features.
  virtual double absolute_signature_weight( rgrl_feature_sptr other ) const;

  //:  Compute the signature error vector between two features.
  virtual vnl_vector<double> signature_error_vector( rgrl_feature const& other ) const;

  //:  the dimensions of the signature error vector.
  virtual unsigned signature_error_dimension( const vcl_type_info& other_feature_type ) const;

 protected:
  //:
  // Create an uninitialized face_pt of dimension dim
  //
  rgrl_feature_face_pt( );

  // to be able to use the protected constructor
  friend rgrl_feature_sptr
         rgrl_feature_reader( vcl_istream& is );

  //: Apply transformation to the scale property
  virtual double
  transform_scale( rgrl_transformation const& xform ) const;

  //:  The location, the normal, and the error projector.
  vnl_vector<double> normal_;
  mutable vnl_matrix<double> err_proj_, err_proj_sqrt_;

 private:

  //:  The basis for the subspace of vectors normal to the normal direction.
  //   This is tangent subspace.  It is computed once, when first needed, and cached.
  //   This is because the feature
  //  location and tangent are fixed.
  bool subspace_cached_;
  vnl_matrix< double > tangent_subspace_;
};


#endif
