#ifndef rgrl_trans_reduced_quad_h_
#define rgrl_trans_reduced_quad_h_
//:
// \file
// \brief Derived class to represent an reduced quadratic transformation in 2D arbitrary dimensions.
// \author Charlene Tsai
// \date Sep 2003

#include "rgrl_transformation.h"
#include <vcl_iosfwd.h>

//:
// A reduced quadratic transformation, when centered, consists of a
// similarity transform + 2 2nd-order radial terms. Uncentering the transform
// makes it look like a quadratic transform
//
class rgrl_trans_reduced_quad
  : public rgrl_transformation
{
 public:
  //: Initialize to the identity transformation.
  //
  rgrl_trans_reduced_quad( unsigned int dimension );

  //: Construct uncentered quadratic standard transform
  //
  //  The transform is q = \a Q * hot(p) + \a A * p + \a trans, where
  //  hot(p)= [px^2 py^2 pxpy]^t in 2D, and
  //  hot(p)= [px^2 py^2 pz^2 pxpy pypz pxpz]^t in 3D.
  //
  rgrl_trans_reduced_quad( vnl_matrix<double> const& Q,
                           vnl_matrix<double> const& A,
                           vnl_vector<double> const& trans,
                           vnl_matrix<double> const& covar );

  //: Construct uncentered quadratic standard transform with unknown covar
  //
  //  The transform is q = \a Q * hot(p) + \a A * p + \a trans, where
  //  hot(p)= [px^2 py^2 pxpy]^t in 2D, and
  //  hot(p)= [px^2 py^2 pz^2 pxpy pypz pxpz]^t in 3D.
  //
  //  The covariance matrix is a zero matrix.
  //
  rgrl_trans_reduced_quad( vnl_matrix<double> const& Q,
                           vnl_matrix<double> const& A,
                           vnl_vector<double> const& trans );

  //: Construct a centered quadratic transform.
  //
  //  The transform is q = \a Q * hot(p) + \a A * p + \a trans, where
  //  hot(p)= [px^2 py^2 pxpy]^t in 2D, and
  //  hot(p)= [px^2 py^2 pz^2 pxpy pypz pxpz]^t in 3D.
  //
  rgrl_trans_reduced_quad( vnl_matrix<double> const& Q,
                           vnl_matrix<double> const& A,
                           vnl_vector<double> const& trans,
                           vnl_matrix<double> const& covar,
                           vnl_vector<double> const& from_centre,
                           vnl_vector<double> const& to_centre );

  void set_from_center( vnl_vector<double> const& from_center );

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const;

  //:  Provide the covariance matrix of the centered estimate (scale is factored in)
  //
  //   For the centered transformation A, the covar matrix is for the parameter
  //   vector [c d a b tx ty]^t, where c and d are for the 2nd order terms,
  //   and a, b, tx and ty are for the similarity transform.
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
  virtual void inv_map( const vnl_vector<double>& to,
                        bool initialize_next,
                        const vnl_vector<double>& to_delta,
                        vnl_vector<double>& from,
                        vnl_vector<double>& from_next_est) const;

  //: Return the jacobian of the transform
  vnl_matrix<double> jacobian( vnl_vector<double> const& from_loc ) const;

  //:  transform the transformation for images of different resolution
  virtual rgrl_transformation_sptr scale_by( double scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_reduced_quad, rgrl_transformation );

  //: Output UNCENTERED transformation and the original center.
  void write(vcl_ostream& os ) const;

  // for input
  void read(vcl_istream& is );

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to ) const;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir    ) const;
 private:
  //: Return the vector of 2nd order terms of p = [x y]^t
  vnl_vector<double> higher_order_terms(vnl_vector<double> p) const;

 private:
  vnl_matrix<double> Q_;
  vnl_matrix<double> A_;
  vnl_vector<double> trans_;
  vnl_vector<double> from_centre_;

  // TODO - pure virtual functions of rgrl_transformation
  virtual void inv_map(vnl_vector<double> const&, vnl_vector<double>&) const;
  virtual rgrl_transformation_sptr inverse_transform() const;
};


#endif
