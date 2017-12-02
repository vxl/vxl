#ifndef rgrl_transformation_h_
#define rgrl_transformation_h_
//:
// \file
// \brief Base class for transformation representation, estimations and application in generalized registration library
// \author Chuck Stewart
// \date 15 Nov 2002

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_compiler.h>

#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_set_of.h>
#include <rgrl/rgrl_match_set_sptr.h>

#include <rgrl/rgrl_transformation_sptr.h>

//: A base class that represents a transformation.
//
// Derived classes must implement
// - map_loc() to map a location
// - map_dir() to map a direction
// - covar() to provide the parameter covariance matrix
//
// We use different names for the functions that derived classes
// implement because otherwise the "name hiding rule" of the C++
// standard is invoked, which means the some of the functionality in
// the base class interface--in particular, the overloaded
// convenience functions--are unavailable in the derived class
// interface.
//
class rgrl_transformation
  : public rgrl_object
{
 public:
  virtual ~rgrl_transformation();

  //: default constructor
  rgrl_transformation() :  is_covar_set_(false) {  }

  //: initialize with covariance matrix
  rgrl_transformation( const vnl_matrix<double>& cov );

  //: Apply the transformation to create a new (mapped) location
  void map_location( vnl_vector<double> const& from,
                     vnl_vector<double>      & to    ) const;

  //:  Apply the transformation to create a new (mapped) location
  //
  // Convenience call to the two parameter version that allocates the
  // result vector.
  //
  vnl_vector<double> map_location( vnl_vector<double> const& p ) const;

  //: Map a tangent direction
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  virtual void map_tangent( vnl_vector<double> const& from_loc,
                            vnl_vector<double> const& from_dir,
                            vnl_vector<double>      & to_dir    ) const;

  //: Map a normal direction
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  virtual void map_normal( vnl_vector<double> const & from_loc,
                           vnl_vector<double> const & from_dir,
                           vnl_vector<double>       & to_dir    ) const;

  //: Map a normal direction, given the tangent subspace
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  virtual void map_normal( vnl_vector<double> const  & from_loc,
                           vnl_vector<double> const  & from_dir,
                           vnl_matrix< double > const& tangent_subspace,
                           vnl_vector<double>        & to_dir    ) const;


  //: Map an arbitrary direction which is neither a tangent nor a normal
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  void map_direction( vnl_vector<double> const& from_loc,
                      vnl_vector<double> const& from_dir,
                      vnl_vector<double>      & to_dir    ) const;

#if 0
  // Don't provide this interface because it is quite easy to confuse
  // it with the three vector version. If you intended to call the
  // three vector version, but forget to provide the location, then
  // the compiler will simply call this version and there will be no
  // error; just wrong results.
  vnl_vector<double> map_direction( vnl_vector<double> const& from_loc,
                                    vnl_vector<double> const& from_dir ) const;
#endif // 0

  //:  Apply to an intensity, with a default of the identity.
  virtual
  double map_intensity( vnl_vector<double> const& from, double intensity ) const;

  //:  Compute covariance of the transfer error based on transformation covariance
  //
  // This gives the additional uncertainty of the transferred point
  // location due to the uncertainty of the transform estimate.
  //
  virtual
  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p ) const = 0;


  //:  Inverse map with an initial guess
  virtual void inv_map( const vnl_vector<double>& to,
                        bool initialize_next,
                        const vnl_vector<double>& to_delta,
                        vnl_vector<double>& from,
                        vnl_vector<double>& from_next_est) const;

  //:  Parameter covariance matrix
  vnl_matrix<double> covar() const;

  //: log of determinant of the covariance
  virtual double
  log_det_covar() const;

  //:  set parameter covariance matrix
  void set_covar( const vnl_matrix<double>& covar );

  //: is covariance set?
  bool is_covar_set() const { return is_covar_set_; }

  //:  Inverse map based on the transformation.
  //   This function only exist for certain transformations.
  virtual void inv_map( const vnl_vector<double>& to,
                        vnl_vector<double>& from ) const;

  //: is this an invertible transformation?
  virtual bool is_invertible() const { return false; }

  //: Return an inverse transformation
  //  This function only exist for certain transformations.
  virtual rgrl_transformation_sptr inverse_transform() const;

  //: Return the jacobian of the transform
  //  use only as backward compatibility
  vnl_matrix<double> jacobian( vnl_vector<double> const& from_loc ) const;

  //: Compute jacobian w.r.t. location
  virtual void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const = 0;

  //:  transform the transformation for images of different resolution
  virtual rgrl_transformation_sptr scale_by( double scale ) const = 0;

  //: output transformation
  virtual void write( std::ostream& os ) const;

  //: input transformation
  virtual bool read( std::istream& is );

  //: make a clone copy
  virtual rgrl_transformation_sptr clone() const=0;

  //: set scaling factors
  //  Unless the transformation is not estimated using estimators in rgrl,
  //  it does not need to be set explicitly
  void set_scaling_factors( vnl_vector<double> const& scaling );

  //: return scaling factor
  const vnl_vector<double>& scaling_factors() const { return scaling_factors_; }

  // Defines type-related functions
  rgrl_type_macro( rgrl_transformation, rgrl_object );


 protected:

  //:  Apply the transformation to create a new (mapped) location
  //
  virtual
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to    ) const = 0;

  //:  Apply the transformation to create a new direction at the (mapped) location
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  virtual
  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const = 0;

  //: a slightly better way of computing log of determinant
  double
  log_det_sym_matrix( vnl_matrix<double> const& m ) const;

  //: compute the log of determinant of the covariance when the matrix is rank deficient
  double
  log_det_covar_deficient( int rank ) const;

 protected:

  //: covariance matrix
  //  Unlike transformation parameters, covariance is always a mtrix of double.
  vnl_matrix<double> covar_;

  //: flag of setting covariance
  //  Check it before using covariance matrix
  bool is_covar_set_;

  //: scaling factors of current transformation on each dimension
  //  This is computed from current transformation.
  //  And it has nothing to do with how to transform points
  vnl_vector<double> scaling_factors_;
};

std::ostream&
operator<< (std::ostream& os, rgrl_transformation const& xform );

#endif
