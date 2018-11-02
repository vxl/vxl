#ifndef rgrl_trans_quadratic_h_
#define rgrl_trans_quadratic_h_
//:
// \file
// \brief Derived class to represent a quadratic transformation in arbitrary dimensions.
// \author Charlene Tsai
// \date Sep 2003

#include <iostream>
#include <iosfwd>
#include "rgrl_transformation.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgrl_trans_quadratic
  : public rgrl_transformation
{
 public:
  //: Initialize to the identity transformation.
  //
  rgrl_trans_quadratic( unsigned int dimension = 0);

  //: Construct quadratic standard transform
  //
  //  The transform is q = \a Q * hot(p) + \a A * p + \a trans, where
  //  hot(p)= [px^2 py^2 pxpy]^t in 2D, and
  //  hot(p)= [px^2 py^2 pz^2 pxpy pypz pxpz]^t in 3D.
  //  See covar() for the
  //  ordering of the covariance matrix.
  //
  rgrl_trans_quadratic( vnl_matrix<double> const& Q,
                        vnl_matrix<double> const& A,
                        vnl_vector<double> const& trans,
                        vnl_matrix<double> const& covar );

  //: Construct quadratic standard transform
  //
  //  The transform is q = \a Q * hot(p) + \a A * p + \a trans, where
  //  hot(p)= [px^2 py^2 pxpy]^t in 2D, and
  //  hot(p)= [px^2 py^2 pz^2 pxpy pypz pxpz]^t in 3D.
  //  See covar() for the
  //  ordering of the covariance matrix.
  //
  //  The covariance matrix is a zero matrix.
  //
  rgrl_trans_quadratic( vnl_matrix<double> const& Q,
                        vnl_matrix<double> const& A,
                        vnl_vector<double> const& trans );

  void
  set_from_center( vnl_vector<double> const& from_center );

  //: Construct a centered quadratic transform.
  //
  //  The transform is
  //  q = \a Q * hot( p - \a from_center) + \a A * ( p - \a from_centre ) +
  //      \a trans + \a to_centre.
  //
  //  See covar() for the ordering of the covariance matrix.
  //
  rgrl_trans_quadratic( vnl_matrix<double> const& Q,
                        vnl_matrix<double> const& A,
                        vnl_vector<double> const& trans,
                        vnl_matrix<double> const& covar,
                        vnl_vector<double> const& from_centre,
                        vnl_vector<double> const& to_centre );


  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const override;

  //:  Provide the covariance matrix of the estimate (scale is factored in)
  //
  //   The rows of the [ Q_ A_]  matrix are of the form a_i^T containing the
  //   coefficients of [x^2 y^2 z^2 xy yz xz x y z]. The assumed form of the
  //   covar matrix is for a vector
  //     ( a_1^T, trans[0], ... , a_n^T, trans[n] )^T
  //
  // defined in base class
  // vnl_matrix<double> covar() const;

  //: The 2nd-order component of the quadratic transform
  vnl_matrix<double> const& Q() const;

  //: The 1st-order component of the quadratic transform
  vnl_matrix<double> const& A() const;

  //: The translation component of the quadratic transform
  vnl_vector<double> const& t() const;

  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                        bool initialize_next,
                        const vnl_vector<double>& to_delta,
                        vnl_vector<double>& from,
                        vnl_vector<double>& from_next_est) const override;

  //: Compute jacobian w.r.t. location
  void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const override;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_quadratic, rgrl_transformation );

  //: Output UNCENTERED transformation, with the origin as the center.
  void write(std::ostream& os ) const override;

  // for input
  bool read(std::istream& is ) override;

  //: make a clone copy
  rgrl_transformation_sptr clone() const override;

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to ) const override;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const override;
 private:
  //: Return the vector of 2nd order terms of p = [x y]^t
  vnl_vector<double> higher_order_terms(vnl_vector<double> p) const;

 private:
  vnl_matrix<double> Q_;
  vnl_matrix<double> A_;
  vnl_vector<double> trans_;
  vnl_vector<double> from_centre_;

  // TODO - pure virtual functions of rgrl_transformation
  void inv_map(vnl_vector<double> const&, vnl_vector<double>&) const override;
  rgrl_transformation_sptr inverse_transform() const override;
};

#endif
