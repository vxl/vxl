#ifndef rgrl_trans_homo2d_proj_rad_h_
#define rgrl_trans_homo2d_proj_rad_h_
//:
// \file
// \author Gehua Yang
// \date   March 2007
// \brief  Represents a 2D homography plus radial distortion transformation.

#include <iostream>
#include <iosfwd>
#include "rgrl_transformation.h"
#include <rgrl/rgrl_est_proj_rad_func.h>
#include <vnl/vnl_matrix_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Represents a 2D homography plus radial distortion transformation.
//  A transformation for x'_u=Hx, and x'_d = x'_u + k*radial_dist( x'_u )
class rgrl_trans_homo2d_proj_rad
  : public rgrl_transformation,
    public rgrl_est_proj_rad_func<2, 2>
{
  vnl_matrix_fixed<double,3,3> H_;
  std::vector<double>           rad_k_;
 public:
  //: Initialize to the identity matrix
  rgrl_trans_homo2d_proj_rad();

  //: Constructor based on an initial transformation and covar estimate
  //  The radial parameters are assumed to be unnormalized.
  rgrl_trans_homo2d_proj_rad( vnl_matrix_fixed<double, 3, 3> const& H,
                              std::vector<double>             const & k,
                              vnl_vector_fixed< double, 2 >  const & image_centre );

  //: Constructor based on an initial transformation and unknown covar
  //  The covariance matrix is a zero matrix.
  rgrl_trans_homo2d_proj_rad( vnl_matrix<double> const& H );

  //: Construct a centered transform.
  //  The radial parameters are assumed to be unnormalized.
  rgrl_trans_homo2d_proj_rad( vnl_matrix_fixed<double, 3, 3> const& H,
                              std::vector<double>             const & k,
                              vnl_vector_fixed< double, 2 >  const & image_centre,
                              vnl_matrix<double> const& covar,
                              vnl_vector<double> const& from_centre,
                              vnl_vector<double> const& to_centre );

  //: Constructor based on an initial transformation and covar estimate
  //  The radial parameters are assumed to be normalized.
  rgrl_trans_homo2d_proj_rad( std::vector<double>             const & norm_k,
                              vnl_matrix_fixed<double, 3, 3> const& H,
                              vnl_vector_fixed< double, 2 >  const & image_centre );

  //: Construct a centered transform.
  //  The radial parameters are assumed to be normalized.
  rgrl_trans_homo2d_proj_rad( std::vector<double>             const & norm_k,
                              vnl_matrix_fixed<double, 3, 3> const& H,
                              vnl_vector_fixed< double, 2 >  const & image_centre,
                              vnl_matrix<double> const& covar,
                              vnl_vector<double> const& from_centre,
                              vnl_vector<double> const& to_centre );

  vnl_matrix<double> transfer_error_covar( vnl_vector<double> const& p  ) const override;

  //: Compute jacobian w.r.t. location
  void jacobian_wrt_loc( vnl_matrix<double>& jac, vnl_vector<double> const& from_loc ) const override;

  //:  transform the transformation for images of different resolution
  rgrl_transformation_sptr scale_by( double scale ) const override;

  //: log of determinant of the covariance
  double
  log_det_covar() const override
  { return log_det_covar_deficient( 8+camera_dof_ ); }

  //: Defines type-related functions
  rgrl_type_macro( rgrl_trans_homo2d_proj_rad, rgrl_transformation );

  //: for output UNCENTERED transformation, with the origin as the center.
  void write(std::ostream& os ) const override;

  //: for input
  bool read(std::istream& is ) override;

  //: make a clone copy
  rgrl_transformation_sptr clone() const override;

  //: The scaling and rotation component of the transform
  vnl_matrix_fixed<double, 3, 3> H() const;

  //: return unnormalized radial parameters
  std::vector<double>
  radial_params() const;

  //: return the normalized radial parameters
  std::vector<double> const&
  normalized_radial_params() const
  { return rad_k_; }

 protected:
  void map_loc( vnl_vector<double> const& from,
                vnl_vector<double>      & to  ) const override;

  void map_dir( vnl_vector<double> const& from_loc,
                vnl_vector<double> const& from_dir,
                vnl_vector<double>      & to_dir  ) const override;

  void set_up_rad_k(std::vector<double> const & rad_k);
};

#endif // rgrl_trans_homo2d_proj_rad_h_
