#ifndef rgrl_transformation_h_
#define rgrl_transformation_h_
//:
// \file
// \brief Base class for transformation representation, estimations and application in generalized registration library
// \author Chuck Stewart
// \date 15 Nov 2002

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vcl_iosfwd.h>

#include "rgrl_object.h"

#include "rgrl_transformation_sptr.h"

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

  //:  Apply the transformation to create a new (mapped) location
  //
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


  // Don't provide this interface because it is quite easy to confuse
  // it with the three vector version. If you intended to call the
  // three vector version, but forget to provide the location, then
  // the compiler will simply call this version and there will be no
  // error; just wrong results.
  //
  //vnl_vector<double> map_direction( vnl_vector<double> const& from_loc,
  //                                  vnl_vector<double> const& from_dir ) const;

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
                        vnl_vector<double>& from_next_est) const ;

  //:  Parameter covariance matrix
  virtual vnl_matrix<double> covar() const = 0;

  //:  Inverse map based on the transformation.
  //   This function only exist for certain transformations.
  virtual void inv_map( const vnl_vector<double>& to,
                        vnl_vector<double>& from ) const;

  //: Return the jacobian of the transform
  virtual vnl_matrix<double> jacobian( vnl_vector<double> const& from_loc ) const = 0;

  //:  transform the transformation for images of different resolution
  virtual rgrl_transformation_sptr scale_by( double scale ) const;

  //: output transformation
  virtual void write( vcl_ostream& os ) const = 0;

  //: input transformation
  virtual void read( vcl_istream& is ) = 0;

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
};


#endif
