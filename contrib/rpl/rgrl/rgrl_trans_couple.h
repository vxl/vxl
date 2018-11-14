#ifndef rgrl_trans_couple_h_
#define rgrl_trans_couple_h_
//:
// \file
// \brief class to encapsulate a pair of transformations: forward & backward
// \author Gehua Yang
// \date Feb 2005

#include <iostream>
#include <iosfwd>
#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_transformation.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A base class that represents a pair of transformation: forward & backward
//
class rgrl_trans_couple
  : public rgrl_transformation
{
 public:
  ~rgrl_trans_couple() override;

  //: default constructor
  rgrl_trans_couple() = default;

  //: initialize with covariance matrix
  rgrl_trans_couple( rgrl_transformation_sptr const& forward, rgrl_transformation_sptr const& backward );


  //: Map a tangent direction
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  void map_tangent( vnl_vector<double> const& from_loc,
                            vnl_vector<double> const& from_dir,
                            vnl_vector<double>      & to_dir    ) const override;

  //: Map a normal direction
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  void map_normal( vnl_vector<double> const & from_loc,
                           vnl_vector<double> const & from_dir,
                           vnl_vector<double>       & to_dir    ) const override;

  //: Map a normal direction, given the tangent subspace
  //
  // The resulting direction \a to_dir is a unit vector.
  //
  void map_normal( vnl_vector<double> const  & from_loc,
                           vnl_vector<double> const  & from_dir,
                           vnl_matrix< double > const& tangent_subspace,
                           vnl_vector<double>        & to_dir    ) const override;

  //:  Compute covariance of the transfer error based on transformation covariance
  //
  // This gives the additional uncertainty of the transferred point
  // location due to the uncertainty of the transform estimate.
  //

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p ) const override;


  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                        bool initialize_next,
                        const vnl_vector<double>& to_delta,
                        vnl_vector<double>& from,
                        vnl_vector<double>& from_next_est) const override;

  //:  Inverse map based on the transformation.
  //   This function only exist for certain transformations.
  void inv_map( const vnl_vector<double>& to,
                        vnl_vector<double>& from ) const override;

  //: is this an invertible transformation?
  bool is_invertible() const override;

  //: Return an inverse transformation
  //  This function only exist for certain transformations.
  rgrl_transformation_sptr inverse_transform() const override;

  //: Compute jacobian w.r.t. location
  void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const override;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const override;

  //: output transformation
  void write( std::ostream& os ) const override;

  //: input transformation
  bool read( std::istream& is ) override;

  //: make a clone copy
  rgrl_transformation_sptr clone() const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_couple, rgrl_transformation );

 protected:

  //:  Apply the transformation to create a new (mapped) location
  //

  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to    ) const override;

  //:  Apply the transformation to create a new direction at the (mapped) location
  //
  // The resulting direction \a to_dir is a unit vector.
  //

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const override;

 protected:

  rgrl_transformation_sptr forward_xform_, backward_xform_;
};

#endif
