#ifndef rgrl_feature_h_
#define rgrl_feature_h_
//:
// \file
// \brief Base class for feature in generalized registration library
// \author Chuck Stewart
// \date 12 Nov 2002

#include <vcl_cassert.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include "rgrl_feature_sptr.h"
#include "rgrl_object.h"
#include <vcl_iosfwd.h>

class rgrl_transformation;

//: Represents a feature ("data point") used by the registration algorithms.
//
class rgrl_feature
  : public rgrl_object
{
 public:
  //:
  virtual ~rgrl_feature() { };

  //:  Apply a transformation to create a new feature.
  virtual
  rgrl_feature_sptr transform( rgrl_transformation const& xform ) const = 0;

  //:  Provide the geometric location.
  virtual
  vnl_vector<double> const& location() const = 0;

  //: Provide the scale level at which this feature is detected
  //  If no associated scale, return 0
  virtual 
  double scale() const { return 0.0; }
  
  //: Set the scale level at which this feature is detected
  virtual 
  void set_scale( double scale ); 
  
  //: read in feature
  virtual 
  bool read( vcl_istream& is, bool skip_tag=false ) = 0;
  
  //: write out feature
  virtual
  void write( vcl_ostream& os ) const = 0;
  
  //:  Projects the error to the normal space of the underlying surface.
  //
  // This matrix essentially describes the underlying surface from
  // which this feature arises. For a normal point, this would be the
  // identity matrix, implying that all dimensions of the error vector
  // are significant. For a point on a curve, this matrix could remove
  // this component of the error in the tangential direction, because
  // we are only interested in the error normal to the curve.
  //
  // Defaults to the identity matrix.
  //
  virtual
  vnl_matrix<double> const& error_projector() const = 0;

  //: Number of constraints provided when another feature matches to it
  virtual
  unsigned int num_constraints() const = 0;

  //:  Compute the geometric error distance between two features.
  virtual 
  double geometric_error( rgrl_feature const& mapped_other ) const;

  //:  Compute the geometric error distance between two features.
  //   Use this function for efficiency. 
  //   If a mapped feature is created, use the other function
  virtual 
  double geometric_error( rgrl_transformation const& xform, 
                          rgrl_feature const& other ) const;

  //:  when computing geometric error, allow only mapping of From location
  //   Otherwise, a mapped feature will be created, which is much more heavy
  virtual
  bool allow_fast_computation_on_error() const { return true; }
  
  //:  Compute the signature error vector between two features.
  //
  // The result is invalid if signature_error_dimension() is false (0).
  //
  // A signature vector stores a set of properties associated with the
  // feature. Take the trace point on a vessel for example. The
  // possible properties include the normal to the vessel and the
  // vessel width at the give trace point. A feature can return
  // different type of signature_error_vector, depending on the type
  // of the \a other feature.
  //
  // A signature_error_vector describes the "difference" between two
  // features. It is important for the computation of robust
  // signature/similarity weight. The robust weight depends on the
  // weight distribution of all matches involved in the estimation of
  // the transformation.
  //
  // CAVEAT: We assume the signature_error_vector has zero-mean.
  //
  virtual vnl_vector<double> signature_error_vector( rgrl_feature const& other ) const;

  //:  If non-zero, the dimensions of the signature error vector.
  //
  // The dimension depends on the \a other feature type. Defaults to 0.
  //
  virtual unsigned signature_error_dimension( const vcl_type_info& other_feature_type ) const;

  //:  Compute the signature weight between two features.
  //
  // The weight is determined solely based on the current and \a other
  // features. In other words, it is independent of the
  // signature/similarity weight distribution of all matches involved
  // in the estimation of the transformation. It is often independent
  // of the \a signature_error_vector. Defaults to 1.
  //
  virtual double absolute_signature_weight( rgrl_feature_sptr /*other*/ ) const
  { return 1.0; }

  // Defines type-related functions
  rgrl_type_macro( rgrl_feature, rgrl_object );

 protected:
  friend rgrl_feature_sptr
         rgrl_feature_reader( vcl_istream& is );

 private:
  // disabled
  rgrl_feature& operator=( rgrl_feature const& );
};


//: Down cast from rgrl_feature_sptr.
//
// This does a dynamic_cast and then asserts that the result is not
// null. Therefore, you are guaranteed that the result is a valid
// pointer, or else the program will halt.
//
// \code
//   rgrl_feature_sptr ptr;
//   rgrl_feature_landmark* real_ptr = rgrl_feature_caster<rgrl_feature_landmark>(ptr);
// \endcode
template<class CastTo>
class rgrl_feature_caster
{
 public:
  rgrl_feature_caster( rgrl_feature_sptr f )
    : data_( dynamic_cast<CastTo*>( f.as_pointer() ) ) { assert( data_ ); }

  operator CastTo*() const { return data_; }
  CastTo* operator ->() const { return data_; }
 private:
  CastTo* data_;
};

#endif
