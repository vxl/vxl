#ifndef rgrl_trans_affine_h_
#define rgrl_trans_affine_h_
//:
// \file
// \brief Derived class to represent an affine transformation in arbitrary dimensions.
// It can be built from lower order (subgroup) transformations so that many
// different transformations can be described.
// \author Chuck Stewart
// \date 15 Nov 2002

#include "rgrl_transformation.h"
#include <vcl_iosfwd.h>

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

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const;

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
                vnl_vector<double>& from_next_est) const;

  //:  Inverse map based on the transformation.
  //   The inverse mapping for A(p)+ t = q is p = A^-1(q-t)
  void inv_map( const vnl_vector<double>& to,
                vnl_vector<double>& from ) const;

  //: is this an invertible transformation?
  virtual bool is_invertible() const { return true; }

  //: Return an inverse transformation
  //  This function only exist for certain transformations.
  virtual rgrl_transformation_sptr inverse_transform() const;

  //: Return the jacobian of the transform
  vnl_matrix<double> jacobian( vnl_vector<double> const& from_loc ) const;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_affine, rgrl_transformation );

  //: Output CENTERED transformation and the original center.
  virtual void write(vcl_ostream& os ) const;

  // for input
  virtual void read(vcl_istream& is );

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to ) const;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const;

 private:
  vnl_matrix<double> A_;
  vnl_vector<double> trans_;
  vnl_vector<double> from_centre_;
};

#endif
