#ifndef rgrl_trans_homography2d_h_
#define rgrl_trans_homography2d_h_
//:
// \file
// \author Charlene Tsai
// \date   Oct 2004

#include "rgrl_transformation.h"
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_iosfwd.h>

//: Represents a 2D homography transformation.
//
//  A transformation for x'=Hx. It is for 2D only.

class rgrl_trans_homography2d
  : public rgrl_transformation
{
 public:
  //: Initialize to the identity matrix
  rgrl_trans_homography2d();

  //: Constructor based on an initial transformation and covar estimate
  //
  rgrl_trans_homography2d( vnl_matrix<double> const& H,
                           vnl_matrix<double> const& covar );

  //: Constructor based on an initial transformation and unknown covar
  //
  //  The  covariance matrix is a zero matrix.
  rgrl_trans_homography2d( vnl_matrix<double> const& H );

  //: Construct a centered transform.
  //
  rgrl_trans_homography2d( vnl_matrix<double> const& H,
                           vnl_matrix<double> const& covar,
                           vnl_vector<double> const& from_centre,
                           vnl_vector<double> const& to_centre );

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const;

  //: The scaling and rotation component of the transform
  vnl_matrix_fixed<double, 3, 3> H() const;

  //: Inverse map using pseudo-inverse of H_.
  void inv_map( const vnl_vector<double>& to,
                vnl_vector<double>& from ) const;

  //:  Inverse map with an initial guess
  void inv_map( const vnl_vector<double>& to,
                bool initialize_next,
                const vnl_vector<double>& to_delta,
                vnl_vector<double>& from,
                vnl_vector<double>& from_next_est) const;

  //: is this an invertible transformation?
  bool is_invertible() const { return true; }

  //: Return an inverse transformation of the uncentered transform
  rgrl_transformation_sptr inverse_transform() const;

  //: Compute jacobian w.r.t. location
  virtual void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const;

  //: Return the jacobian of the transform. This is a 2x3 matrix
  vnl_matrix_fixed<double,2,3> homo_jacobian( vnl_vector_fixed<double,2> const& from_loc ) const;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_trans_homography2d, rgrl_transformation );

  // for output UNCENTERED transformation, with the origin as the center.
  void write(vcl_ostream& os ) const;

  // for input
  void read(vcl_istream& is );

  // uncenter H matrix
  vnl_matrix_fixed<double,3,3> uncenter_H_matrix() const;

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to  ) const;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir  ) const;

 private:
  vnl_matrix_fixed<double,3,3> H_;
  vnl_vector_fixed<double,2>   from_centre_;
  vnl_vector_fixed<double,2>   to_centre_;
};

#endif
