#ifndef rgrl_trans_affine_h_
#define rgrl_trans_affine_h_
//:
// \file
// \brief Derived class to represent an affine transformation in arbitrary dimensions.
// It can be built from lower order (subgroup) transformations so that many
// different transformations can be described.
// \author Chuck Stewart
// \date 15 Nov 2002

#include <iostream>
#include <iosfwd>
#include "rgrl_transformation.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rgrl_trans_affine
  : public rgrl_transformation
{
 public:
  //: Initialize to the identity transformation.
  //
  rgrl_trans_affine( unsigned int dimension = 0);

  //: Construct affine standard transform
  //
  //  The transform is q = \a A * p + \a trans. See covar() for the
  //  ordering of the covariance matrix.
  //
  rgrl_trans_affine( vnl_matrix<double> const& A,
                     vnl_vector<double> const& trans,
                     vnl_matrix<double> const& covar );

  //: Construct affine standard transform
  //
  //  The transform is q = \a A * p + \a trans.
  //
  rgrl_trans_affine( vnl_matrix<double> const& A,
                     vnl_vector<double> const& trans,
                     vnl_vector<double> const& from_centre );

  //: Construct affine standard transform with unknown covariance
  //
  //  The transform is q = \a A * p + \a trans.
  //  The covariance matrix is a zero matrix.
  //
  rgrl_trans_affine( vnl_matrix<double> const& A,
                     vnl_vector<double> const& trans );


  //: Construct a centered affine transform.
  //
  //  The transform is q = \a A * ( p - \a from_centre ) + \a trans + \a to_centre.
  //
  //  See covar() for the ordering of the covariance matrix.
  //
  rgrl_trans_affine( vnl_matrix<double> const& A,
                     vnl_vector<double> const& trans,
                     vnl_matrix<double> const& covar,
                     vnl_vector<double> const& from_centre,
                     vnl_vector<double> const& to_centre );

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const override;

  //:  Provide the covariance matrix of the estimate (scale is factored in)
  //
  //   If the rows of the A_ matrix are of the form a_i^T, then the assumed
  //   form of the covar matrix is for a vector
  //     ( a_1^T, trans[0], ... , a_n^T, trans[n] )^T
  //
  // defined in base class
  // vnl_matrix<double> covar() const;

  //: The non-translation component of the affine transform
  vnl_matrix<double> const& A() const;

  //: The translation component of the affine transform
  vnl_vector<double> t() const;

  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                bool initialize_next,
                const vnl_vector<double>& to_delta,
                vnl_vector<double>& from,
                vnl_vector<double>& from_next_est) const override;

  //:  Inverse map based on the transformation.
  //   The inverse mapping for A(p)+ t = q is p = A^-1(q-t)
  void inv_map( const vnl_vector<double>& to,
                vnl_vector<double>& from ) const override;

  //: is this an invertible transformation?
  bool is_invertible() const override { return true; }

  //: Return an inverse transformation
  //  This function only exist for certain transformations.
  rgrl_transformation_sptr inverse_transform() const override;

  //: Compute jacobian w.r.t. location
  void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const override;

  //: Accessor for from_centre_
  vnl_vector<double> const &  from_centre() { return from_centre_; }

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_affine, rgrl_transformation );

  //: Output CENTERED transformation and the original center.
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

  vnl_matrix<double> A_;
  vnl_vector<double> trans_;
  vnl_vector<double> from_centre_;
};

#endif
